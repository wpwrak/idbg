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

static const uint8_t *wbuf, *wpos, *wend;
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
		if (pos < 0)
			usb_send(ep, wbuf+fetched, size, NULL, 0);
		else
			usb_send(ep, rbuf+pos, size, NULL, 0);
		fetched += size;
	}
	return 1;
}


#ifndef SIMULATION

static void delay(void)
{
	int i;

	for (i = 0; i != 240/3; i++)
		__asm nop __endasm;
}

#endif /* !SIMULATION */


void i2c_poll(void)
{
	int8_t i;

	switch (state) {
	case IDLE:
	case FAILED:
		break;
	case START:
		if (!I2C_SDA || !I2C_SCL)
			break;
		I2C_SDA = 0;
		if (wpos == wend)
			data = device << 1 | 1;
		else
			data = device << 1;
		delay();
		state = SEND;
		break;
	case SEND:
		for (i = 7; i != -1; i--) {
			I2C_SCL = 0;
			sda = (data >> i) & 1;
			I2C_SDA = sda;
			delay();
			I2C_SCL = 1;
			delay();
		}
		I2C_SCL = 0;
		delay();
		I2C_SDA = 1;
		state = ACK;
		break;
	case RECV:
		data = 0;
		for (i = 7; i != -1; i--) {
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
		I2C_SCL = 1;
		delay();
		if (wpos != wend) {
			data = *wpos++;
			state = SEND;
		}
		else {
			if (rpos != rend)
				state = RECV;
			else {
				state = IDLE;
				delay();
			}
		}
		break;
	case STOP:
		if (!I2C_SDA)
			break;
		state = IDLE;
		delay();
		break;
	default:
		break;
	}
}
