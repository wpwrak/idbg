/*
 * This code follows the register read/write sequences from the examples in
 * SiLabs/MCU/Examples/C8051F326_7/USB_Interrupt/Firmware/F326_USB_Main.c and 
 * SiLabs/MCU/Examples/C8051F326_7/USB_Interrupt/Firmware/F326_USB_ISR.c
 */

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

	OSCICN |= IFCN0 | IFCN1;

#ifdef LOW_SPEED

	CLKSEL = 0x10;	/* USBCLK = int/2, SYS_INT_OSC = int */

#else /* LOW_SPEED */

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
	CLKMUL |= MULEN;
	delay();
	CLKMUL |= MULINIT | MULEN;
	while (!(CLKMUL & MULRDY));
	CLKSEL = 0;	/* USBCLK = 4*int, SYSCLK = int */
	CLKSEL = 0x02;	/* F326_USB_Main.c does this (sets 24MHz). Why ? */

#endif /* !LOW_SPEED */

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

	uart_init();
	printk("%s #%u\n", build_date, build_number);
	usb_init();
	while (1);
}
