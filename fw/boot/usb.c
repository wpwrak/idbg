/*
 * Known issues:
 * - no suspend/resume
 * - doesn't handle packet fragmentation yet (so low-speed doesn't work)
 */

#include <stdint.h>

#include "regs.h"
#include "uart.h"
#include "usb.h"


#ifndef NULL
#define NULL 0
#endif


#define BUG_ON(x)


#define N 2
#define NO_ADDRESS	0xff	/* null value for function address */


const uint8_t *ep0 = NULL, *ep0_end;
uint8_t addr = NO_ADDRESS;


static void trigger(void)
{
	static uint16_t count = 0;

	if (++count == N) {
		P2_0 = 0;
		P2_0 = 1;
		putchar('*');
	}
}


static void usb_write(uint8_t reg, uint8_t value)
{
	while (USB0ADR & BUSY);
	USB0ADR = reg;
	USB0DAT = value;
}


static uint8_t usb_read(uint8_t reg)
{
	while (USB0ADR & BUSY);
	USB0ADR = reg | BUSY;
	while (USB0ADR & BUSY);
	return USB0DAT;
}


static uint16_t usb_read_word(uint8_t reg)
{
	uint8_t low;

	low = usb_read(reg);
	return low | usb_read(reg);
}


static bit get_descriptor(uint8_t type, uint8_t index, uint16_t length)
{
	uint8_t size;

	debug("get_descriptor(0x%02x, 0x%02x, 0x%04x)\n", type, index, length);

	switch (type) {
	case USB_DT_DEVICE:
//putchar('D');
		ep0 = device_descriptor;
		break;
	case USB_DT_CONFIG:
		if (index) {
putchar('?');
			return 0;
		}
//putchar('C');
//printk("%d", length);
trigger();
		ep0 = config_descriptor;
		break;
	default:
		return 0;
	}
//	size = *ep0;
//	if (length < size)
		size = length;
	ep0_end = ep0+size;
	return 1;
}


/*
 * Process a SETUP packet. Hardware ensures that length is 8 bytes.
 */


static void setup(void)
{
	uint8_t bmRequestType, bRequest;
	uint16_t wValue, wIndex, wLength;
	bit ok = 0;

//	printk("fifo=%d\n", usb_read(E0CNT));
	BUG_ON(usb_read(E0CNT) < 8);

	bmRequestType = usb_read(FIFO0);
	bRequest = usb_read(FIFO0);
	wValue = usb_read_word(FIFO0);
	wIndex = usb_read_word(FIFO0);
	wLength = usb_read_word(FIFO0);

	switch (bmRequestType | bRequest << 8) {

	/*
	 * Device request
	 *
	 * See http://www.beyondlogic.org/usbnutshell/usb6.htm
	 */

	case FROM_DEVICE(GET_STATUS):
		debug("GET_STATUS\n");
		if (wLength != 2)
			goto stall;
		ep0 = "\000";
		ep0_end = ep0+2;
		break;
	case TO_DEVICE(CLEAR_FEATURE):
		printk("CLEAR_FEATURE\n");
		ok = 1;
		break;
	case TO_DEVICE(SET_FEATURE):
		debug("SET_FEATURE\n");
		break;
	case TO_DEVICE(SET_ADDRESS):
		debug("SET_ADDRESS (0x%x)\n", wValue);
//		putchar('A');
//		printk("A=%x\n", wValue);
		addr = wValue;
		ok = 1;
		break;
	case FROM_DEVICE(GET_DESCRIPTOR):
		ok = get_descriptor(wValue, wValue >> 8, wLength);
		break;
	case TO_DEVICE(SET_DESCRIPTOR):
		printk("SET_DESCRIPTOR\n");
		break;
	case FROM_DEVICE(GET_CONFIGURATION):
		printk("GET_CONFIGURATION\n");
		break;
	case TO_DEVICE(SET_CONFIGURATION):
		debug("SET_CONFIGURATION\n");
		ok = wValue == config_descriptor[5];
		break;

	/*
	 * Interface request
	 */

	case FROM_INTERFACE(GET_STATUS):
		printk("GET_STATUS\n");
		break;
	case TO_INTERFACE(CLEAR_FEATURE):
		printk("CLEAR_FEATURE\n");
		break;
	case TO_INTERFACE(SET_FEATURE):
		printk("SET_FEATURE\n");
		break;
	case FROM_DEVICE(GET_INTERFACE):
		printk("GET_INTERFACE\n");
		break;
	case TO_DEVICE(SET_INTERFACE):
		debug("SET_INTERFACE\n");
		{
			uint8_t *interface_descriptor = config_descriptor+9;

			ok = wIndex == interface_descriptor[2] &&
			    wValue == interface_descriptor[3];
		}
		break;

	/*
	 * Endpoint request
	 */

	case FROM_ENDPOINT(GET_STATUS):
		printk("GET_STATUS\n");
		break;
	case TO_ENDPOINT(CLEAR_FEATURE):
		printk("CLEAR_FEATURE(EP)\n");
		break;
	case TO_ENDPOINT(SET_FEATURE):
		printk("SET_FEATURE(EP)\n");
		break;
	case FROM_ENDPOINT(SYNCH_FRAME):
		printk("SYNCH_FRAME\n");
		break;

	default:
		printk("Unrecognized SETUP(%02x %02x ...\n",
		    bmRequestType, bRequest);
	}

	if (ok) {
		if (bmRequestType & 0x80)
			usb_write(E0CSR, SOPRDY);
		else
			usb_write(E0CSR, SOPRDY | DATAEND);
		debug("OK");
		return;
	}
stall:
	printk("STALL\n");
	usb_write(E0CSR, SDSTL);
}


static void handle_ep0(void)
{
	uint8_t csr;

	if (addr != NO_ADDRESS) {
//		printk("[%d]\n", addr);
		usb_write(FADDR, addr);
		putchar('A');
		addr = NO_ADDRESS;
	}

	csr = usb_read(E0CSR);

	/* clear sent stall indication */
	if (csr & STSTL)
		usb_write(E0CSR, 0);

	/* if transaction was interrupted, clean up */
	if (csr & SUEND) {
		putchar('S');
		usb_write(E0CSR, DATAEND | SSUEND);
		ep0 = NULL;
	}

	if (csr & OPRDY)
		setup();

	if (!ep0)
		return;

	csr = usb_read(E0CSR);
	if (csr & INPRDY)
		return;
	if (csr & (SUEND | OPRDY))
		return;

	while (ep0 != ep0_end)
		usb_write(FIFO0, *ep0++);

	ep0 = NULL;
	csr |= INPRDY | DATAEND;

	usb_write(E0CSR, csr);
}


static void poll(void)
{
	uint8_t flags;

	while (1) {
		flags = usb_read(CMINT);
		if (flags) {
			debug("CMINT 0x%02x\n", flags);
			if (flags & RSTINT)
				usb_write(POWER, 0);
				    /* @@@ 1 for suspend signaling */
		}

		flags = usb_read(IN1INT);
		if (flags) {
			debug("IN1INT 0x%02x\n", flags);
			if (flags & EP0)
				handle_ep0();
			if (flags & IN1)
				/* handle IN */;
		}

		flags = usb_read(OUT1INT);
		if (flags) {
			debug("OUT1INT 0x%02x\n", flags);
			/* handle OUT */
		}
    }
}


void usb_init(void)
{
	/* from the example in F326_USB_ISR.c */
	usb_write(POWER, 8);
#ifdef LOW_SPEED
	USB0XCN = 0xc0;
	usb_write(CLKREC, 0xa0);
#else
	USB0XCN = 0xe0;
	usb_write(CLKREC, 0x80);
#endif
	//usb_write(POWER, 0x01);	/* we don't implement suspend yet */
	usb_write(POWER, 0x00);
	poll();

#if 0
	//usb_write(POWER, USBRST | USBINH);	/* reset USB0 */
	usb_write(POWER, USBRST | USBINH);	/* reset USB0 */
	USB0XCN = PHYEN | SPEED;	/* configure and enable transceiver */
	usb_write(POWER, 0);		/* enable USB0 */
	// usb_write(CLKREC, 0x09 | CRE | CRSSEN);
					/* enable USB clock recovery */
	usb_write(CLKREC, 0x00 | CRE);	/* enable USB clock recovery */
	USB0XCN |= PREN;		/* enable pull-up */

	USB0XCN &= ~PREN;		/* disable pull-up */
#endif
}
