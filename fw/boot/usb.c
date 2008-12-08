#include <stdint.h>

#include "regs.h"
#include "uart.h"
#include "usb.h"


#define BUG_ON(x)


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


static void usb_reply(const void *d, uint8_t size)
{
	uint8_t i;

	for (i = 0; i != size; i++) {
		debug("%02x ", ((const uint8_t *) d)[i]);
		usb_write(FIFO0, ((const uint8_t *) d)[i]);
	}
}


static bit get_descriptor(uint8_t type, uint8_t index, uint16_t length)
{
	const void *p;
	uint8_t size;

	debug("get_descriptor(0x%02x, 0x%02x, 0x%04x)\n", type, index, length);

	switch (type) {
	case USB_DT_DEVICE:
putchar('D');
		p = device_descriptor;
		break;
	case USB_DT_CONFIG:
putchar('C');
		if (index)
			return 0;
		p = config_descriptor;
		break;
	default:
		return 0;
	}
	size = *(const uint8_t *) p;
	if (length < size)
		size = length;
	usb_reply(p, size);
	return 1;
}


/*
 * Process a SETUP packet. Hardware ensures that length is 8 bytes.
 */


static void setup(void)
{
	static uint8_t addr = 0xff;
	uint8_t bmRequestType, bRequest;
	uint16_t wValue, wIndex, wLength;
	bit ok = 0;
	uint8_t csr, ack = DATAEND | SOPRDY;

	if (addr != 0xff) {
//		printk("[%d]\n", addr);
		usb_write(FADDR, addr);
		putchar('A');
		addr = 0xff;
	}

	csr = usb_read(E0CSR);
	if (csr & SUEND) {
		putchar('S');
		usb_write(E0CSR, SSUEND);
	}
	if (!(csr & OPRDY))
		return;

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
		usb_reply("\000", 2);
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
	P2_0 = 0;
	P2_0 = 1;
		addr = wValue;
		ok = 1;
		ack |= INPRDY;
		break;
	case FROM_DEVICE(GET_DESCRIPTOR):
		ok = get_descriptor(wValue, wValue >> 8, wLength);
		ack |= INPRDY;
		break;
	case TO_DEVICE(SET_DESCRIPTOR):
		printk("SET_DESCRIPTOR\n");
		break;
	case FROM_DEVICE(GET_CONFIGURATION):
		printk("GET_CONFIGURATION\n");
		break;
	case TO_DEVICE(SET_CONFIGURATION):
		printk("SET_CONFIGURATION\n");
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
		printk("SET_INTERFACE\n");
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
		usb_write(E0CSR, ack);
		debug("OK");
		return;
	}
stall:
	printk("STALL\n");
	usb_write(E0CSR, SDSTL);
}


static void poll(void)
{
	uint8_t flags;

	while (1) {
		flags = usb_read(CMINT);
		if (flags) {
			debug("CMINT 0x%02x\n", flags);
			if (flags & RSTINT)
				usb_write(POWER, 1);
		}

		flags = usb_read(IN1INT);
		if (flags) {
			debug("IN1INT 0x%02x\n", flags);
			if (flags & EP0)
				setup();
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
