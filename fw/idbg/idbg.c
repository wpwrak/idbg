/*
 * idbg/idbg.c - IDBG initialization and main loop
 *
 * Written 2008, 2009 by Werner Almesberger
 * Copyright 2008, 2009 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "regs.h"
#include "uart.h"
#include "usb.h"
#include "serial.h"
#include "i2c.h"
#include "ep0.h"
#include "version.h"


void uart_isr(void) __interrupt(4);


void main(void)
{
	/* @@@ since boot and payload may run at different USB speeds, we
	   should also redo the clock init */
	uart_init(24);

	printk("Hello, payload\n");
	printk("%s #%u\n", build_date, build_number);

	EA = 1;

/*
 * @@@ known issue: USB enumerates fine, but we have to explicitly reset the
 * bus before we can communicate, which is odd. Disconnect (i.e., turning off
 * the pull-up) should be detected within 2-2.5us, so the problem seems to be
 * something else.
 */

	usb_init();
	ep0_init();
	serial_init();
	while (1) {
		usb_poll();
		serial_poll();
		i2c_poll();
	}
}
