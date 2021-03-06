/*
 * common/uart.c - UART initialization and debug output
 *
 * Written 2008, 2009 by Werner Almesberger
 * Copyright 2008, 2009 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdarg.h>
#include <stdio.h>

#include "regs.h"
#include "uart.h"


/* gpio j4=0 */


#ifndef CONFIG_USB_PUTCHAR

void putchar(char c)
{
	if (c == '\n')
		putchar('\r');
	SBUF0 = c;
	while (!TI0);
	TI0 = 0;
}

#endif /* !CONFIG_USB_PUTCHAR */


/* for now, we always use printf_tiny, which doesn't have a vprintf* version */
#ifdef noCONFIG_PRINTK

void printk(const char *fmt, ...)
{
	va_list ap;
	__bit saved;

	va_start(ap, fmt);
	saved = EA;
	EA = 0;
	vprintf(fmt, ap);
	EA = saved;
	va_end(ap);
}

#endif /* CONFIG_PRINTK */


void uart_init(uint8_t brg_mhz)
{
	/*
	 * UART: Enable only transmitter, no interrupts.
	 */
	SCON0 = 0; /* also clears TI0 */
	SMOD0 = S0DL1 | S0DL0;

	/*
	 * Configure the UART to 115200bps, see table 13.1
	 *
	 * We can's support 1.5MHz (that is, without using the USB clock, but
	 * why would one want to run the core at 1.5MHz when USB is around ?)
	 *
	 * The closes settings would be:
	 *
	 * SBRL0 = 0xfff9; -- 107142.9 bps, error = -7%
	 * SBRL0 = 0xfffa; --  125000 bps, error = +8.5%
	 *
	 * Depending on signal quality, we would need something like +/-5%.
	 */

#if defined(UART_115200_BPS)
	switch (brg_mhz) {
	case 3:
		SBRL0 = 0xfff3;
		break;
	case 6:
		SBRL0 = 0xffe6;
		break;
	case 12:
		SBRL0 = 0xffcc;
		break;
	case 24:
		SBRL0 = 0xff98;
		break;
	case 48:
		SBRL0 = 0xff30;
		break;
	}
#elif defined(UART_57600_BPS)
	switch (brg_mhz) {
	case 3:
		SBRL0 = 0xffe6;
		break;
	case 6:
		SBRL0 = 0xffcc;
		break;
	case 12:
		SBRL0 = 0xff98;
		break;
	case 24:
		SBRL0 = 0xff30;
		break;
	case 48:
		SBRL0 = 0xfe5f;
		break;
	}
#else
#error "must set either UART_115200_BPS or UART_57600_BPS"
#endif

	SBCON0 = SB0RUN | SB0PS0 | SB0PS1;

	/*
	 * Make TX a push-pull output
	 */
	P0MDOUT |= 1 << 4;
}
