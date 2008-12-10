#include <stdint.h>

#include "version.h"
#include "regs.h"
#include "uart.h"
#include "usb.h"


static void delay(void)
{
	int x;

	for (x = 0; x < 500; x)
		x++;
}

void main(void)
{
	/*
	 * @@@ if we don't have VBUS, proceed as follows:
	 * - stay at 3MHz (current < 2mA)
	 * - jump directly to the payload
	 */

	/*
	 * If we have VBUS, proceed as follows:
	 * - bring up USB
	 * - try to contact the PMU (in a loop)
	 * - possible transitions:
	 *   - DFU gets selected -> enter DFU mode
	 *   - PMU responds -> jump to payload
	 *
	 * In DFU mode, the following transitions are possible:
	 * - VBUS drops -> reset
	 * - USB bus reset -> reset
	 */

	/*
	 * Note: if we do anything that delays CPU bringup after nRESET goes
	 * high, we must still stay in the 100ms budget for raising KEEPACT.
	 */

	/* straight from the example in F326_USB_Main.c */
#ifdef LOW_SPEED
	OSCICN |= 0x03;
	CLKSEL = 0; // SYS_INT_OSC
	CLKSEL |= 0x10; // USB_INT_OSC_DIV_2
#else
	OSCICN |= 0x03;
	CLKMUL = 0;
	CLKMUL |= 0x80;
	delay();
	CLKMUL |= 0xc0;
	while(!(CLKMUL & 0x20));
	CLKSEL = 0; // SYS_INT_OSC
	CLKSEL |= 0x00; // USB_4X_CLOCK
	CLKSEL = 0x02;
#endif

#if 0

	/*
	 * Make sure we're running with clock divider /8
	 * Thus, SYSCLK = 1.5MHz
	 */
	OSCICN &= ~(IFCN0 | IFCN1);

	/*
	 * Clock multiplier enable sequence, section 10.4
	 *
	 * - reset the multiplier
	 * - select the multiplier input source
	 * - enable the multiplier
	 * - delay for 5us
	 * - initialize the multiplier
	 * - poll for multiplier to be ready
	 */
	CLKMUL = 0;
#if 1
	CLKMUL |= MULEN;
    
	// cycles(5);	/* 4*1.5us = 6us */
	CLKMUL |= MULEN;
	CLKMUL |= MULEN;
	CLKMUL |= MULEN;
	CLKMUL |= MULEN;

	CLKMUL |= MULINIT | MULEN;
	while (!(CLKMUL & MULRDY));
#endif

	/* Set SYSCLK to 12MHz */
	OSCICN |= IFCN1 | IFCN0;
	CLKSEL = 0;		/* USBCLK = 4 x int., SYSCLK = int. */

	/*
	 * VDD monitor enable sequence, section 7.2
	 *
	 * - enable voltage monitor
	 * - wait for monitor to stabilize
	 * - enable VDD monitor reset
	 */
	VDM0CN = VDMEN;
	while (!(VDM0CN & VDDSTAT));
	RSTSRC = PORSF;

	/*
	 * @@@ if VBUS && AUX -> DFU
	 * @@@ else -> boot payload
	 */
#endif

	uart_init();
	printk("%s #%u\n", BUILD_DATE, BUILD_NUMBER);
	usb_init();
	while (1);
}
