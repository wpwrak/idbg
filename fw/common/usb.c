/*
 * Known issues:
 * - no suspend/resume
 * - doesn't handle packet fragmentation yet (so low-speed doesn't work)
 */

/*
 * This code follows the register read/write sequences from the examples in
 * SiLabs/MCU/Examples/C8051F326_7/USB_Interrupt/Firmware/F326_USB_Main.c and
 * SiLabs/MCU/Examples/C8051F326_7/USB_Interrupt/Firmware/F326_USB_ISR.c
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


__xdata struct ep_descr ep0;

bit (*user_setup)(struct setup_request *setup) __reentrant;
bit (*user_get_descriptor)(uint8_t type, uint8_t index,
    const uint8_t * const *reply, uint8_t *size) __reentrant;
void (*user_reset)(void) __reentrant;


static uint8_t addr = NO_ADDRESS;


void usb_io(struct ep_descr *ep, enum ep_state state, uint8_t *buf,
    uint8_t size, void (*callback)(void *user), void *user)
{
	BUG_ON(ep->state);
	ep->state = state;
	ep->buf = buf;
	ep->end = buf+size;
	ep->callback = callback;
	ep->user = user;
}


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
	return low | usb_read(reg) << 8;
}


static bit get_descriptor(uint8_t type, uint8_t index, uint16_t length)
{
	const uint8_t *reply;
	uint8_t size;

	debug("get_descriptor(0x%02x, 0x%02x, 0x%04x)\n", type, index, length);

	switch (type) {
	case USB_DT_DEVICE:
		reply = device_descriptor;
		size = reply[0];
		break;
	case USB_DT_CONFIG:
		if (index)
			return 0;
		reply = config_descriptor;
		size = reply[2];
		break;
	default:
		if (!user_get_descriptor)
			return 0;
		if (!user_get_descriptor(type, index, &reply, &size))
			return 0;
	}
	if (length < size)
		size = length;
	usb_send(&ep0, reply, size, NULL, NULL);
	return 1;
}


/*
 * Process a SETUP packet. Hardware ensures that length is 8 bytes.
 */


static void handle_setup(void)
{
	struct setup_request setup;
	bit ok = 0;

	BUG_ON(usb_read(E0CNT) < 8);

	setup.bmRequestType = usb_read(FIFO0);
	setup.bRequest = usb_read(FIFO0);
	setup.wValue = usb_read_word(FIFO0);
	setup.wIndex = usb_read_word(FIFO0);
	setup.wLength = usb_read_word(FIFO0);

	switch (setup.bmRequestType | setup.bRequest << 8) {

	/*
	 * Device request
	 *
	 * See http://www.beyondlogic.org/usbnutshell/usb6.htm
	 */

	case FROM_DEVICE(GET_STATUS):
		debug("GET_STATUS\n");
		if (setup.wLength != 2)
			goto stall;
		usb_send(&ep0, "\000", 2, NULL, NULL);
		ok = 1;
		break;
	case TO_DEVICE(CLEAR_FEATURE):
		debug("CLEAR_FEATURE\n");
		ok = 1;
		break;
	case TO_DEVICE(SET_FEATURE):
		debug("SET_FEATURE\n");
		break;
	case TO_DEVICE(SET_ADDRESS):
		debug("SET_ADDRESS (0x%x)\n", setup.wValue);
		addr = setup.wValue;
		ok = 1;
		break;
	case FROM_DEVICE(GET_DESCRIPTOR):
		ok = get_descriptor(setup.wValue >> 8, setup.wValue,
		    setup.wLength);
		break;
	case TO_DEVICE(SET_DESCRIPTOR):
		error("SET_DESCRIPTOR\n");
		break;
	case FROM_DEVICE(GET_CONFIGURATION):
		debug("GET_CONFIGURATION\n");
		usb_send(&ep0, "", 1, NULL, NULL);
		ok = 1;
		break;
	case TO_DEVICE(SET_CONFIGURATION):
		debug("SET_CONFIGURATION\n");
		ok = setup.wValue == config_descriptor[5];
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
	case FROM_INTERFACE(GET_INTERFACE):
		printk("GET_INTERFACE\n");
		break;
	case TO_INTERFACE(SET_INTERFACE):
		debug("SET_INTERFACE\n");
		{
			uint8_t *interface_descriptor = config_descriptor+9;

			ok = setup.wIndex == interface_descriptor[2] &&
			    setup.wValue == interface_descriptor[3];
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
		if (user_setup) {
			ok = user_setup(&setup);
			if (ok)
				break;
		}
		printk("Unrecognized SETUP(%02x %02x ...\n",
		    setup.bmRequestType, setup.bRequest);
	}

	if (ok) {
		if ((setup.bmRequestType & 0x80) || ep0.state == EP_RX)
			usb_write(E0CSR, SOPRDY);
		else
			usb_write(E0CSR, SOPRDY | DATAEND);
		debug("OK");
		return;
	}
stall:
	printk("STALL\n");
	usb_write(E0CSR, SDSTL);
	ep0.state = EP_STALL;
}


static void ep0_data(void)
{
	uint8_t fifo;

	fifo = usb_read(E0CNT);
	if (fifo > ep0.end-ep0.buf) {
		usb_write(E0CSR, SDSTL);
		return;
	}
	while (fifo--)
		*ep0.buf++ = usb_read(FIFO0);
	if (ep0.buf == ep0.end) {
		ep0.state = EP_IDLE;
		if (ep0.callback)
			ep0.callback(ep0.user);
	}
	if (ep0.state == EP_IDLE)
		usb_write(E0CSR, SOPRDY | DATAEND);
	else
		usb_write(E0CSR, SOPRDY);
}


static void handle_ep0(void)
{
	uint8_t csr, size, left;

	if (addr != NO_ADDRESS) {
		usb_write(FADDR, addr);
//		putchar('A');
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
		ep0.state = EP_IDLE;
	}

	if (csr & OPRDY) {
		switch (ep0.state) {
		case EP_IDLE:
			handle_setup();
			break;
		case EP_RX:
			ep0_data();
			break;
		default:
			printk("???\n");
			break;
		}
	}

	if (ep0.state != EP_TX)
		return;

	csr = usb_read(E0CSR);
	if (csr & INPRDY)
		return;
	if (csr & (SUEND | OPRDY))
		return;

	size = ep0.end-ep0.buf;
	if (size > EP0_SIZE)
		size = EP0_SIZE;
	for (left = size; left; left--)
		usb_write(FIFO0, *ep0.buf++);

	csr |= INPRDY;
	if (size < EP0_SIZE) {
		ep0.state = EP_IDLE;
		csr |= DATAEND;
	}

	usb_write(E0CSR, csr);

	if (ep0.state == EP_IDLE && ep0.callback)
		ep0.callback(ep0.user);
}


void usb_poll(void)
{
	uint8_t flags;

	flags = usb_read(CMINT);
	if (flags) {
		debug("CMINT 0x%02x\n", flags);
		if (flags & RSTINT) {
			ep0.state = EP_IDLE;
			usb_write(POWER, 0);
			if (user_reset)
				user_reset();
			    /* @@@ 1 for suspend signaling */
		}
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


void usb_init(void)
{
	usb_write(POWER, USBRST);
#ifdef LOW_SPEED
	USB0XCN = PHYEN | PREN;
	usb_write(CLKREC, CRE | CRLOW);
#else
	USB0XCN = PHYEN | PREN | SPEED;
	usb_write(CLKREC, CRE);
#endif
	//usb_write(POWER, 0x01);	/* we don't implement suspend yet */
	usb_write(POWER, 0x00);
}