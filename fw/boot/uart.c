#include <stdarg.h>
#include <stdio.h>

#include "regs.h"
#include "uart.h"


/* gpio j4=0 */


void putchar(char c)
{
	if (c == '\n')
		putchar('\r');
	SBUF0 = c;
	while (!TI0);
	TI0 = 0;
}


void printk(const char *fmt, ...)
{
	va_list ap;
	bit saved;

	va_start(ap, fmt);
	saved = EA;
	EA = 0;
	vprintf(fmt, ap);
	EA = saved;
	va_end(ap);
}


void uart_init(void)
{
	/*
	 * UART: Enable only transmitter, no interrupts.
	 */
	SCON0 = 0; /* also clears TI0 */
	SMOD0 = S0DL1 | S0DL0;

	/*
	 * Configure the UART to 115200bps, see table 13.1
	 */
#ifdef LOW_SPEED
	SBRL0 = 0xffcc;
#else
	SBRL0 = 0xff98;
#endif
	SBCON0 = SB0RUN | SB0PS0 | SB0PS1;

	/*
	 * Make TX a push-pull output
	 */
	P0MDOUT |= 1 << 4;
}
