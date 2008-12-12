/*
 * A few, erm, shortcuts:
 *
 * - we don't bother with the app* states since DFU is all this firmware does
 * - after DFU_DNLOAD, we just block until things are written, so we never
 *   enter dfuDNLOAD_SYNC or dfuDNBUSY
 * - no dfuMANIFEST_SYNC, dfuMANIFEST, or dfuMANIFEST_WAIT_RESET
 * - to keep our buffers small, we only accept EP0-sized blocks
 */


#include <stdint.h>

#include "regs.h"
#include "uart.h"
#include "usb.h"
#include "dfu.h"


#ifndef NULL
#define NULL 0
#endif


#undef debug
#define debug printk


const uint8_t device_descriptor[] = {
	18,			/* bLength */
	USB_DT_DEVICE,		/* bDescriptorType */
	LE(0x100),		/* bcdUSB */
	USB_CLASS_PER_INTERFACE,/* bDeviceClass */
	0x00,			/* bDeviceSubClass (per interface) */
	0x00,			/* bDeviceProtocol (per interface) */
	EP0_SIZE,		/* bMaxPacketSize */
	LE(USB_VENDOR),		/* idVendor */
	LE(USB_PRODUCT),	/* idProduct */
	LE(0x0001),		/* bcdDevice */
	0,			/* iManufacturer */
	0,			/* iProduct */
	0,			/* iSerialNumber */
	1			/* bNumConfigurations */
};


const uint8_t config_descriptor[] = {
	9,			/* bLength */
	USB_DT_CONFIG,		/* bDescriptorType */
	LE(9+9),		/* wTotalLength */
	1,			/* bNumInterfaces */
	1,			/* bConfigurationValue (> 0 !) */
	0,			/* iConfiguration */
//	USB_ATTR_SELF_POWERED | USB_ATTR_BUS_POWERED,
	USB_ATTR_BUS_POWERED,	/* bmAttributes */
	15,			/* bMaxPower */

	/* Interface #0 */

	9,			/* bLength */
	USB_DT_INTERFACE,	/* bDescriptorType */
	0,			/* bInterfaceNumber */
	0,			/* bAlternateSetting */
	0,			/* bNumEndpoints */
	0xfe,			/* bInterfaceClass (application specific) */
	0x01,			/* bInterfaceSubClass (device fw upgrade) */
	0x02,			/* bInterfaceProtocol (DFU mode protocol) */
	0,			/* iInterface */
};


static const uint8_t functional_descriptor[] = {
	9,			/* bLength */
	DFU_DT_FUNCTIONAL,	/* bDescriptorType */
	0xf,			/* bmAttributes (claim omnipotence :-) */
	LE(0xffff),		/* wDetachTimeOut (we're very patient) */
	LE(EP0_SIZE),		/* wTransferSize */
	LE(0x101),		/* bcdDFUVersion */
};


static struct dfu {
	uint8_t status;		/* bStatus */
	uint8_t toL, toM, toH;	/* bwPollTimeout */
	uint8_t state;		/* bState */
	uint8_t iString;
} dfu = {
	OK,
	LE(1000), 0,
	dfuIDLE,
	0,
};


static uint16_t next_block = 0;
static uint16_t payload;


#define	PAYLOAD_START	0x2000
#define	PAYLOAD_END	0x4000


static uint8_t buf[EP0_SIZE];


static void flash_erase_page(uint16_t addr)
{
	FLKEY = 0xa5;
	FLKEY = 0xf1;
	PSCTL |= PSEE;
	PSCTL |= PSWE;
	*(__xdata uint8_t *) addr = 0;
	PSCTL &= ~PSWE;
	PSCTL &= ~PSEE;
}


static void flash_write_page(uint16_t addr, uint8_t value)
{
	FLKEY = 0xa5;
	FLKEY = 0xf1;
	PSCTL |= PSWE;
	PSCTL &= ~PSEE;
	*(__xdata uint8_t *) addr = value;
	PSCTL &= ~PSWE;
}


static void block_write(void *user)
{
	uint8_t *p;

	user; /* suppress warning */
	for (p = buf; p != buf+sizeof(buf); p++) {
		if (!(payload & 511))
			flash_erase_page(payload);
		flash_write_page(payload, *p);
		payload++;
	}
}


static bit block_receive(uint16_t length)
{
	if (payload < PAYLOAD_START || payload+length > PAYLOAD_END) {
		dfu.state = dfuERROR;	
		dfu.status = errADDRESS;
		return 0;
	}
	if (length > EP0_SIZE) {
		dfu.state = dfuERROR;	
		dfu.status = errUNKNOWN;
		return 0;
	}
	usb_recv(&ep0, &buf, sizeof(buf), block_write, NULL);
	return 1;
}


static bit my_setup(struct setup_request *setup) __reentrant
{
	bit ok;

	switch (setup->bmRequestType | setup->bRequest << 8) {
	case DFU_TO_DEV(DFU_DETACH):
		printk("DFU_DETACH\n");
		/* protocol 1 only */
		break;
	case DFU_TO_DEV(DFU_DNLOAD):
		debug("DFU_DNLOAD\n");
		if (dfu.state == dfuIDLE) {
			next_block = 0;
			payload = PAYLOAD_START;
		}
		else if (dfu.state != dfuDNLOAD_IDLE)
			return 0;
		if (next_block && setup->wValue == next_block-1) {
			debug("retransmisson\n");
			return 1;
		}
		if (setup->wValue != next_block) {
			debug("bad block\n");
			return 0;
		}
		if (!setup->wLength) {
			debug("DONE\n");
			dfu.state = dfuIDLE;
			return 1;
		}
		ok = block_receive(setup->wLength);
		next_block++;
		dfu.state = dfuDNLOAD_IDLE;
		return ok;
	case DFU_FROM_DEV(DFU_UPLOAD):
		printk("DFU_UPLOAD\n");
		break;
	case DFU_FROM_DEV(DFU_GETSTATUS):
		debug("DFU_GETSTATUS\n");
		usb_send(&ep0, &dfu, sizeof(dfu), NULL, NULL);
		return 1;
	case DFU_TO_DEV(DFU_CLRSTATUS):
		debug("DFU_CLRSTATUS\n");
		dfu.state = dfuIDLE;
		dfu.status = OK;
		return 1;
	case DFU_FROM_DEV(DFU_GETSTATE):
		debug("DFU_GETSTATE\n");
		usb_send(&ep0, &dfu.state, 1, NULL, NULL);
		return 1;
	case DFU_TO_DEV(DFU_ABORT):
		debug("DFU_ABORT\n");
		dfu.state = dfuIDLE;
		dfu.status = OK;
		return 1;
	default:
printk("0x%x =? 0x%x %u\n",
	setup->bmRequestType | setup->bRequest << 8,
 DFU_FROM_DEV(DFU_GETSTATUS), DFU_GETSTATUS);
		return 0;
	}
	return 0;
}


static bit my_descr(uint8_t type, uint8_t index, const uint8_t **reply,
    uint8_t *size) __reentrant
{
	index; /* suppress warning */
	if (type != DFU_DT_FUNCTIONAL)
		return 0;
	*reply = functional_descriptor;
	*size = sizeof(functional_descriptor);
	return 1;
}


void dfu_init(void)
{
	user_setup = my_setup;
	user_get_descriptor = my_descr;
}
