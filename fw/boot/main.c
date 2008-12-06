#include <stdint.h>
#include <mcs51/C8051F326.h>

#include "regs.h"
#include "uart.h"


#define BIT	P0_4


static void usb_write(uint8_t reg, uint8_t value)
{
    while (USB0ADR & BUSY);
    USB0ADR = reg;
    USB0DAT = value;
}


void main(void)
{
    /*
     * @@@ soft reset -> DFU
     */

    /*
     * USB Full Speed Clock Settings, section 10.5.2
     *
     * -> SYSCLK = 12 MHz 
     */

    CLKMUL = 0;			/* disable clock multiplier */
    OSCICN |= IFCN0 | IFCN1;	/* SYSCLK = osc/1 */
    CLKSEL = 0;			/* USBCLK = 4 x SYSCLK */
    usb_write(CLKREC, CRE);	/* enable USB clock recovery */

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

#if 1
    uart_init();
    print("Hello\r\n");
    while (1);
#else
    P0MDOUT |= 1 << 4;
    while (1) {
	BIT = 1;
	BIT = 0;
    }
#endif
}
