/*
 * idbg/i2c.c - I2C protocol engine
 *
 * Written 2008, 2009 by Werner Almesberger
 * Copyright 2008, 2009 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * "The I2C-bus and how to use it"
 * http://www.i2c-bus.org/fileadmin/ftp/i2c_bus_specification_1995.pdf
 * "The I2C-bus specification"
 * http://www.semiconductors.philips.com/acrobat_download/literature/9398/39340011.pdf
 */


#include <stdint.h>

#include "regs.h"
#include "uart.h"
#include "io.h"
#include "usb.h"
#include "i2c.h"


static enum state {
	IDLE,
	FAILED,
	START,	/* try to send S */
	SEND,	/* send a byte */
	RECV,	/* receive a byte */
} state = IDLE;

static const uint8_t *wbuf, *wpos, *wend;
static uint8_t *rbuf, *rpos, *rend;
static uint8_t fetched, spos;
static __bit sda;


void i2c_start(uint8_t s, const uint8_t *wdata, uint8_t wlen,
    uint8_t *rdata, uint8_t rlen)
{
	wbuf = wpos = wdata;
	wend = wbuf+wlen;
	rbuf = rpos = rdata;
	rend = rbuf+rlen;
	fetched = 0;
	spos = s;
	state = START;
}


__bit i2c_fetch(struct ep_descr *ep, uint8_t len)
{
	uint8_t *p;
	uint8_t size;

	if (state == FAILED)
		return 0;
	if (state != IDLE) {
		usb_send(ep, NULL, 0, NULL, 0);
	} else {
		size = rend-rbuf;
		if (size)
			p = rbuf+fetched;
		else {
			p = wbuf+2+fetched;
			size = wend-wbuf-2;
		}
		size -= fetched;
		if (size > len)
			size = len;
		usb_send(ep, p, size, NULL, 0);
		fetched += size;
	}
	return 1;
}


#ifndef SIMULATION

static void delay(void)
{
	uint8_t i;

	/*
	 * Minimum loop time is 4 cycles. Maximum clock is 24MHz. We want to
	 * wait at least 5us (for t(LOW)/(tSU;STA), which are both >= 4.7us.
	 */
	for (i = 24*5/4; i; i--)
		__asm nop __endasm;
}

#endif /* !SIMULATION */


void i2c_poll(void)
{
	int8_t i;
	uint8_t data;
	__bit nak;

	switch (state) {
	case IDLE:
	case FAILED:
		break;
	case START:
		if (!I2C_SDA || !I2C_SCL)
			break;
		I2C_SDA = 0;
		delay();
		I2C_SCL = 0;
		state = SEND;
		delay();
		break;
	case SEND:
		if (spos && wpos-wbuf == spos) {
			I2C_SDA = 1;
			delay();
			I2C_SCL = 1;
			delay();
			if (!I2C_SCL)
				break;
			I2C_SDA = 0;
			spos = 0;
			delay();
			I2C_SCL = 0;
			delay();
		}
		sda = *wpos >> 7;
		I2C_SDA = sda;
		delay();
		I2C_SCL = 1;
		if (!I2C_SCL)
			break;
		delay();
		I2C_SCL = 0;
		for (i = 6; i != -1; i--) {
			sda = (*wpos >> i) & 1;
			I2C_SDA = sda;
			delay();
			I2C_SCL = 1;
			delay();
			I2C_SCL = 0;
		}
		wpos++;
		I2C_SDA = 1;
		delay();
		I2C_SCL = 1;
		delay();
		nak = I2C_SDA;
		I2C_SCL = 0;
		I2C_SDA = 0;
		delay();
#ifdef SIMULATION
		nak = 0;
#endif
		if (nak) {
			state = FAILED;
		} else {
			if (wpos != wend)
				break;
			if (rpos != rend) {
				state = RECV;
				break;
			}
			state = IDLE;
		}
		I2C_SCL = 1;
		delay();
		I2C_SDA = 1;
		delay();
		break;
	case RECV:
		I2C_SDA = 1;
		data = 0;
		for (i = 7; i != -1; i--) {
			delay();
			I2C_SCL = 1;
			delay();
			data |= I2C_SDA << i;
			I2C_SCL = 0;
		}
		*rpos++ = data;
		if (rpos != rend)
			I2C_SDA = 0;
		delay();
		I2C_SCL = 1;
		delay();
		I2C_SCL = 0;
		if (rpos != rend)
			break;

		delay();
		I2C_SDA = 0;
		delay();
		I2C_SCL = 1;
		delay();
		I2C_SDA = 1;
		delay();
		state = IDLE;
		break;
	default:
		break;
	}
}
