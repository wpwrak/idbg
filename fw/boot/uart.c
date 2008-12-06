#include <mcs51/C8051F326.h>

#include "regs.h"
#include "uart.h"


/* gpio j4=0 */


void uart_send_noint(const char *s)
{
    while (*s++) {
	SBUF0 = *s;
	while (!TI0);
	TI0 = 0;
    }
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
    SBRL0 = 0xffcc;
    SBCON0 = SB0RUN | SB0PS0 | SB0PS1;

    /*
     * Make TX an output
     */
    P0MDOUT |= 1 << 4;
}
