/*
 * idbg/i2c.c - I2C protocol engine
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

#include "regs.h"
#include "uart.h"
#include "io.h"
#include "usb.h"
#include "i2c.h"


static enum state {
	IDLE,
	FAILED,
	START,	/* try to send S */
	ACK,	/* wait for ACK */
	STOP,	/* try to send P */
	SEND,	/* send a byte */
	RECV,	/* receive a byte */
} state = IDLE;

static uint8_t *wbuf, *wpos, *wend;
static uint8_t *rbuf, *rpos, *rend;
static uint8_t device, fetched, data;
static __bit sda;


void i2c_start(uint8_t slave, const uint8_t *wdata, uint8_t wlen,
    uint8_t *rdata, uint8_t rlen)
{
	device = slave;
	wbuf = wpos = wdata;
	wend = wbuf+wlen;
	rbuf = rpos = rdata;
	rend = rbuf+rlen;
	fetched = 0;
	state = START;
}


__bit i2c_fetch(struct ep_descr *ep, uint8_t len)
{
	int8_t pos;
	uint8_t size;

	if (state == FAILED)
		return 0;
	if (state != IDLE) {
		usb_send(ep, NULL, 0, NULL, 0);
	} else {
		pos = fetched-(wend-wbuf);
		size = pos < 0 ? -pos : rend-rbuf-pos;
		if (size > len)
			size = len;
		usb_send(ep, wbuf+fetched, size, NULL, 0);
		fetched += size;
	}
	return 1;
}


static void delay(void)
{
	int i;

	for (i = 0; i != 240/3; i++)
		__asm nop __endasm;
}


void i2c_poll(void)
{
	int i;

	switch (state) {
	case IDLE:
	case FAILED:
		break;
	case START:
		if (!I2C_SDA || !I2C_SCL)
			break;
		I2C_SDA = 0;
		if (wpos == wend)
			data = device | 0x80;
		else
			data = device;
		state = SEND;
		break;
	case SEND:
		for (i = 0; i != 8; i++) {
			I2C_SCL = 0;
			sda = (data >> i) & 1;
			I2C_SDA = sda;
			delay();
			I2C_SCL = 1;
			delay();
		}
		state = ACK;
		break;
	case RECV:
		data = 0;
		for (i = 0; i != 8; i++) {
			I2C_SCL = 0;
			delay();
			I2C_SCL = 1;
			delay();
			data |= I2C_SDA << i;
		}
		*rpos++ = data;
		if (rpos != rend)
			I2C_SDA = 0;
		I2C_SCL = 0;
		delay();
		I2C_SCL = 1;
		delay();
		if (rpos == rend)
			state = STOP;
		break;
	case ACK:
		if (I2C_SDA)
			break;
		if (wpos != wend) {
			data = *wpos++;
			state = SEND;
		}
		else {
			if (rpos != rend)
				state = RECV;
			else {
				I2C_SDA = 1;
				state = IDLE;
			}
		}
		break;
	case STOP:
		if (!I2C_SDA)
			break;
		state = IDLE;
	default:
		break;
	}
}
