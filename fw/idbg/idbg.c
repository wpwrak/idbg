/*
 * idbg/idbg.c - IDBG initialization and main loop
 *
 * Written 2008 by Werner Almesberger
 * Copyright 2008 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "regs.h"
#include "uart.h"
#include "usb.h"
#include "ep0idbg.h"
#include "version.h"


static void usb_isr(void) __interrupt(8)
{
	putchar('&');
}

static __xdata uint8_t buf[64];


static void ep1_out(void *user)
{
	uint8_t got, i;

	user; /* silence sdcc */
	got = sizeof(buf)-usb_left(&ep1out);
	printk("ep1_out: %d\n", got);
	for (i = 0; i != got; i++)
		putchar(buf[i]);
	usb_recv(&ep1out, buf, sizeof(buf), ep1_out, NULL);
}


void main(void)
{
	/* @@@ since boot and payload may run at different USB speeds, we
	   should also redo the clock init */
	uart_init(24);

	printk("Hello, payload\n");
	printk("%s #%u\n", build_date, build_number);

//EIE1 = 2;
//EA = 1;

/*
 * @@@ known issue: USB enumerates fine, but we have to explicitly reset the
 * bus before we can communicate, which is odd. Disconnect (i.e., turning off
 * the pull-up) should be detected within 2-2.5us, so the problem seems to be
 * something else.
 */

	ep0idbg_init();
	usb_init();
	usb_recv(&ep1out, buf, sizeof(buf), ep1_out, NULL);
	while (1)
		usb_poll();
}
