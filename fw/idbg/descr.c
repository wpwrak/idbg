/*
 * idbg/descr.c - USB descriptors
 *
 * Written 2008 by Werner Almesberger
 * Copyright 2008 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "usb.h"


/*
 * Device descriptor
 */

const uint8_t device_descriptor[18] = {
	18,			/* bLength */
	USB_DT_DEVICE,		/* bDescriptorType */
	LE(0x200),		/* bcdUSB */
	USB_CLASS_VENDOR_SPEC,	/* bDeviceClass */
	0x00,			/* bDeviceSubClass */
	0x00,			/* bDeviceProtocol */
	EP0_SIZE,		/* bMaxPacketSize */
	LE(0x1234),		/* idVendor */
	LE(0x0002),		/* idProduct */
	LE(0x0001),		/* bcdDevice */
	0,			/* iManufacturer */
	0,			/* iProduct */
	0,			/* iSerialNumber */
	1			/* bNumConfigurations */
};


/*
 * Our configuration
 *
 * While in DFU mode, we're always bus-powered.
 */

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
	USB_CLASS_VENDOR_SPEC,	/* bInterfaceClass */
	0,			/* bInterfaceSubClass */
	0,			/* bInterfaceProtocol */
	0,			/* iInterface */
};
