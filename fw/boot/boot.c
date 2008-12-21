/*
 * This code follows the register read/write sequences from the examples in
 * SiLabs/MCU/Examples/C8051F326_7/USB_Interrupt/Firmware/F326_USB_Main.c and 
 * SiLabs/MCU/Examples/C8051F326_7/USB_Interrupt/Firmware/F326_USB_ISR.c
 */

#include <stdint.h>

#include "version.h"
#include "regs.h"
#include "io.h"
#include "uart.h"
#include "usb.h"
#include "dfu.h"


void run_payload(void)
{
	/* No interrupts while jumping between worlds */
	EA = 0;

	/* Restart USB */
	USB0XCN = 0;

	/* Re-enable pull-ups */
	GPIOCN &= ~WEAKPUD;

	/* Don't waste power in pull-down */
	I2C_SDA_PULL = 1;

	debug("launching payload\n");

	__asm
	ljmp	PAYLOAD_START
	__endasm;
}


/* ----- Interrupts -------------------------------------------------------- */


/*
 * The boot loader doesn't use interrupts, so we forward all interrupts to the
 * payload.
 *
 * What we'd really like to do here is to say something like
 *
 * void whatever_isr(void) __interrupt(n) __at(PAYLOAD+n*8+1);
 *
 * However, sdcc doesn't support such things yet. So we declare the ISR such
 * that the vector entry gets created, and then we tell the assembler where to
 * find it.
 *
 * Since __asm/__endasm isn't allowed outside a function body, we generate a
 * dummy function for each assignment. The function is "naked", so that no
 * actual code is generated for it.
 */

#define	ISR(n) \
	void isr_nr_##n(void) __interrupt(n); \
	void isr_dummy_##n(void) __naked \
	{ \
		__asm \
		_isr_nr_##n = PAYLOAD_START+n*8+3 \
		__endasm; \
	}


ISR(0)
ISR(1)
ISR(2)
ISR(3)
ISR(4)
ISR(8)
ISR(15)


/* ----- The actual boot loader -------------------------------------------- */


static void delay(void)
{
	int x;

	for (x = 0; x < 500; x)
		x++;
}


static void boot_loader(void)
{
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
	 *
	 * @@@ this may be too complex - probably don't really need to talk to
	 * the PMU.
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

	uart_init(24);

#endif /* !LOW_SPEED */

	printk("%s #%u\n", build_date, build_number);

	/*
	 * Very weakly pull SDA down and disable all pull-ups.
	 *
	 * We use SDA as a system power presence detector. When I2C is idle,
	 * SDA is kept high. This is accomplished with pull-ups in the system.
	 * We can therefore detect if IO_3V3 is any good by checking whether
	 * SDA is high.
	 *
	 * This should work even without out own pull-down. However, when the
	 * IDBG board is operating standalone (or, generally, if SDA isn't
	 * connected), SDA would float. We therefore have to pull it down a
	 * little.
	 */

	GPIOCN |= WEAKPUD;
	I2C_SDA_PULL = 0;
	delay();

	dfu_init();
	usb_init();
	while (!I2C_SDA || dfu.state != dfuIDLE)
		usb_poll();
}


void main(void)
{
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
	 * @@@ if we don't have VBUS, proceed as follows:
	 * - stay at 3MHz (current < 2mA, so we're fine with GPIO power)
	 * - jump directly to the payload
	 */

	OSCICN |= IFCN0;
	uart_init(3);
	if (REG0CN & VBSTAT)
		boot_loader();
	run_payload();
}
