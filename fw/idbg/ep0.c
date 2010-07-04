/*
 * idbg/ep0.c - EP0 extension protocol
 *
 * Written 2008-2010 by Werner Almesberger
 * Copyright 2008-2010 Werner Almesberger
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
#include "idbg/ep0.h"


static const uint8_t id[] = { EP0IDBG_MAJOR, EP0IDBG_MINOR };
static __xdata uint8_t buf[EP1_SIZE];
static uint16_t size, jtag_bits;
static __bit jtag_last; /* scan is last segment of larger scan */


#define	UPDATE_MASKED(var, value, mask) \
	var = ((var) & ~(mask)) | ((value) & (mask))


static void do_jtag_scan(void *user)
{
	user; /* suppress warning */
	jtag_scan(buf, jtag_bits, jtag_last);
}


static void do_jtag_move(void *user)
{
	user; /* suppress warning */
	jtag_move(buf, jtag_bits);
}


static void do_i2c_write(void *user)
{
	user; /* suppress warning */
	i2c_start(0, buf, size+2, NULL, 0);
}


#define SET_GPIO(port, bit, value)		\
	case port*4+bit:			\
		P##port##_##bit	= value;	\
		break


static void set_gpio(uint8_t n, uint8_t v)
{
	switch (n) {
		SET_GPIO(0, 0, v);
		SET_GPIO(0, 1, v);
		SET_GPIO(0, 2, v);
		SET_GPIO(0, 3, v);
		SET_GPIO(0, 4, v);
		SET_GPIO(0, 5, v);
		SET_GPIO(0, 6, v);
		SET_GPIO(0, 7, v);
		SET_GPIO(2, 0, v);
		SET_GPIO(2, 1, v);
		SET_GPIO(2, 2, v);
		SET_GPIO(2, 3, v);
		SET_GPIO(2, 4, v);
		SET_GPIO(2, 5, v);
	/*
	 * P0_0 ... P0_7: bits 0 ... 7
	 * P2_0 ... P2_5: bits 8 ... 13
	 * P3_0: bit 14
	 */
		case 14:
			P3_0 = v;
	}
}


static __bit my_setup(struct setup_request *setup) __reentrant
{
	uint8_t i;

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
	case IDBG_TO_DEV(IDBG_JTAG_SCAN):
		debug("IDBG_JTAG_SEND\n");
		if (setup->wLength > EP1_SIZE)
			return 0;
		jtag_bits = setup->wValue;
		size = (jtag_bits+7) >> 3;
		if (size > setup->wLength)
			return 0;
		/*
		 * @@@ Prophylaxis ...
		 */
		{
			volatile uint16_t foo;
			foo = size;
			size = foo;
		}
		jtag_last = setup->wIndex;
		usb_recv(&ep0, buf, setup->wLength, do_jtag_scan, NULL);
		return 1;
	case IDBG_FROM_DEV(IDBG_JTAG_FETCH):
		debug("IDBG_JTAG_FETCH\n");
		if (setup->wLength > EP1_SIZE)
			return 0;
		size = (setup->wValue+7) >> 3;
		/*
		 * @@@ Prophylaxis ...
		 */
		{
			volatile uint16_t foo;
			foo = size;
			size = foo;
		}
		if (size > setup->wLength)
			return 0;
		usb_send(&ep0, jtag_data, size, NULL, NULL);
		return 1;
	case IDBG_TO_DEV(IDBG_JTAG_MOVE):
		debug("IDBG_JTAG_MOVE\n");
		if (setup->wLength > EP1_SIZE)
			return 0;
		jtag_bits = setup->wValue;
		size = (jtag_bits+7) >> 3;
		/*
		 * @@@ SDCC 2.8.0 gets very confused if we don't access "size"
		 * before using it. May be similar to the code generation bug
		 * found in ../boot/dfu.c
		 */
		{
			volatile uint16_t foo;
			foo = size;
			size = foo;
		}
		/*
		 * @@@ The above attempt to bring code generation back to
		 * normality upsets the request code calculation. Let's try the
		 * work-around from ../boot/dfu.c
		 */
		{	
			static volatile uint8_t foo;
			foo = setup->bRequest;
                }

		if (size > setup->wLength)
			return 0;
		usb_recv(&ep0, buf, setup->wLength, do_jtag_move, NULL);
		return 1;

	case IDBG_TO_DEV(IDBG_I2C_WRITE):
		debug("IDBG_I2C_WRITE\n");
		if (setup->wLength > EP1_SIZE-2)
			return 0;
		buf[0] = setup->wIndex << 1;
		buf[1] = setup->wValue;
		size = setup->wLength;
		usb_recv(&ep0, buf+2, size, do_i2c_write, NULL);
		return 1;
	case IDBG_FROM_DEV(IDBG_I2C_READ):
		debug("IDBG_I2C_READ\n");
		buf[0] = setup->wIndex << 1;
		buf[1] = setup->wValue;
		buf[2] = setup->wIndex << 1 | 1;
		i2c_start(2, buf, 3, buf, setup->wLength);
		usb_send(&ep0, NULL, 0, NULL, NULL);
		return 1;
	case IDBG_FROM_DEV(IDBG_I2C_FETCH):
		debug("IDBG_I2C_FETCH\n");
		return i2c_fetch(&ep0, setup->wLength);

	case IDBG_TO_DEV(IDBG_GPIO_DATA_SET):
		debug("IDBG_GPIO_DATA_SET");
		for (i = 0; i != 16; i++)
			if (setup->wIndex & (1 << i))
				set_gpio(i, (setup->wValue >> i) & 1);
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


void ep0_init(void)
{
	user_setup = my_setup;
}
