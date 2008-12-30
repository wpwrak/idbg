/*
 * idbg/ep0idbg.c - EP0 extension protocol
 *
 * Written 2008 by Werner Almesberger
 * Copyright 2008 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>

#ifndef NULL
#define NULL 0
#endif

#include "regs.h"
#include "uart.h"
#include "usb.h"
#include "i2c.h"
#include "jtag.h"
#include "ep0idbg.h"


static const uint8_t id[] = { EP0IDBG_MAJOR, EP0IDBG_MINOR };
static __xdata uint8_t buf[EP0_SIZE];
static uint16_t size, jtag_bits;
static uint8_t i2c_device, i2c_addr;
static uint8_t p0_shadow = 0xff, p2_shadow = 0xff;


#define	UPDATE_MASKED(var, value, mask) \
	var = ((var) & ~(mask)) | ((value) & (mask))


static void do_jtag_send(void *user)
{
	user; /* suppress warning */
	jtag_send(buf, jtag_bits);
}


static void do_i2c_write(void *user)
{
	user; /* suppress warning */
	i2c_start(i2c_device, buf, size, NULL, 0);
}


static __bit my_setup(struct setup_request *setup) __reentrant
{
	switch (setup->bmRequestType | setup->bRequest << 8) {
	case IDBG_FROM_DEV(IDBG_ID):
		debug("IDBG_ID\n");
		if (setup->wLength > 2)
			return 0;
		usb_send(&ep0, id, setup->wLength, NULL, NULL);
		return 1;
	case IDBG_TO_DEV(IDBG_RESET):
		debug("IDBG_RESET\n");
		RSTSRC = SWRSF;
		while (1);

	case IDBG_TO_DEV(IDBG_JTAG_ATTACH):
		debug("IDBG_JTAG_ATTACH\n");
		jtag_attach();
		return 1;
	case IDBG_TO_DEV(IDBG_JTAG_DETACH):
		debug("IDBG_JTAG_DETACH\n");
		jtag_detach();
		return 1;
	case IDBG_TO_DEV(IDBG_JTAG_GPIO_SET):
		debug("IDBG_JTAG_GPIO_SET\n");
		jtag_gpio_set(setup->wValue, setup->wIndex);
		return 1;
	case IDBG_FROM_DEV(IDBG_JTAG_GPIO_GET):
		debug("IDBG_JTAG_GPIO_GET\n");
		*buf = jtag_gpio_get();
		if (setup->wLength)
			usb_send(&ep0, buf, 1, NULL, NULL);
		return 1;
	case IDBG_TO_DEV(IDBG_JTAG_SEND):
		debug("IDBG_JTAG_SEND\n");
		if (setup->wLength > EP0_SIZE)
			return 0;
		jtag_bits = setup->wValue;
		if (jtag_bits >> 8 > setup->wLength)
			return 0;
		usb_recv(&ep0, buf, size, do_jtag_send, NULL);
		return 1;
	case IDBG_FROM_DEV(IDBG_JTAG_FETCH):
		debug("IDBG_JTAG_FETCH\n");
		if (setup->wLength > EP0_SIZE)
			return 0;
		if (setup->wValue >> 8 > setup->wLength)
			return 0;
		usb_send(&ep0, jtag_data, setup->wValue >> 8, NULL, NULL);
		return 1;

	case IDBG_TO_DEV(IDBG_I2C_WRITE):
		debug("IDBG_I2C_WRITE\n");
		if (setup->wLength > EP0_SIZE-1)
			return 0;
		i2c_device = setup->wIndex;
		*buf = setup->wValue;
		size = setup->wLength;
		usb_recv(&ep0, buf+1, size, do_i2c_write, NULL);
		return 1;
	case IDBG_FROM_DEV(IDBG_I2C_READ):
		debug("IDBG_I2C_READ\n");
		i2c_addr = setup->wValue;
		i2c_start(setup->wIndex, &i2c_addr, 1, buf, setup->wLength);
		usb_send(&ep0, NULL, 0, NULL, NULL);
		return 1;
	case IDBG_FROM_DEV(IDBG_I2C_FETCH):
		debug("IDBG_I2C_FETCH\n");
		return i2c_fetch(&ep0, setup->wLength);

	case IDBG_TO_DEV(IDBG_GPIO_UPDATE):
		debug("IDBG_GPIO_UPDATE\n");
		UPDATE_MASKED(p0_shadow, P0, setup->wIndex);
		UPDATE_MASKED(p2_shadow, P2, setup->wIndex >> 8);
		return 1;
	case IDBG_TO_DEV(IDBG_GPIO_DATA_SET):
		debug("IDBG_GPIO_DATA_SET");
		UPDATE_MASKED(p0_shadow, setup->wValue, setup->wIndex);
		UPDATE_MASKED(p2_shadow, setup->wValue >> 8,
		    setup->wIndex >> 8);
		P0 = p0_shadow;
		P2 = p2_shadow;
		return 1;
	case IDBG_FROM_DEV(IDBG_GPIO_DATA_GET):
		debug("IDBG_GPIO_DATA_GET");
		if (setup->wLength > 2)
			return 0;
		buf[0] = P0;
		buf[1] = P2;
		usb_send(&ep0, buf, setup->wLength, NULL, NULL);
		return 1;
	case IDBG_TO_DEV(IDBG_GPIO_MODE_SET):
		debug("IDBG_GPIO_MODE_SET");
		UPDATE_MASKED(P0MDOUT, setup->wValue, setup->wIndex);
		UPDATE_MASKED(P2MDOUT, setup->wValue >> 8, setup->wIndex >> 8);
		return 1;
	case IDBG_FROM_DEV(IDBG_GPIO_MODE_GET):
		debug("IDBG_GPIO_MODE_GET");
		if (setup->wLength > 2)
			return 0;
		buf[0] = P0MDOUT;
		buf[1] = P2MDOUT;
		usb_send(&ep0, buf, setup->wLength, NULL, NULL);
		return 1;

	default:
		error("Unrecognized SETUP: 0x%02x 0x%02x ...\n",
		    setup->bmRequestType, setup->bRequest);
		return 0;
	}
}


void ep0idbg_init(void)
{
	user_setup = my_setup;
}
