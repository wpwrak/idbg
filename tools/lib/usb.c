/*
 * lib/usb.c - Common USB code for IDBG tools
 *
 * Written 2008-2010 by Werner Almesberger
 * Copyright 2008-2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */



#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <usb.h>
//#include <linux/usbdevice_fs.h>

#include "idbg/usb-ids.h"


static uint16_t vendor = 0;
    /* search for USB_VENDOR_OPENMOKO and USB_VENDOR_QI_HW by default */
static uint16_t product = USB_PRODUCT_IDBG;


usb_dev_handle *open_usb(void)
{
	const struct usb_bus *bus;
	struct usb_device *dev;

	usb_init();
	usb_find_busses();
	usb_find_devices();

	for (bus = usb_get_busses(); bus; bus = bus->next)
		for (dev = bus->devices; dev; dev = dev->next) {
			if (!vendor &&
			    dev->descriptor.idVendor != USB_VENDOR_OPENMOKO &&
			    dev->descriptor.idVendor != USB_VENDOR_QI_HW)
				continue;
			if (vendor && dev->descriptor.idVendor != vendor)
				continue;
			if (dev->descriptor.idProduct != product)
				continue;
			return usb_open(dev);
		}
	return NULL;
}


static void bad_id(const char *id)
{
	fprintf(stderr, "\"%s\" is not a valid vendor:product ID\n", id);
	exit(1);
}


void parse_usb_id(const char *id)
{
	unsigned long tmp;
	char *end;

	tmp = strtoul(id, &end, 16);
	if (*end != ':')
		bad_id(id);
	if (tmp > 0xffff)
		bad_id(id);
	vendor = tmp;
	tmp = strtoul(end+1, &end, 16);
	if (*end)
		bad_id(id);
	if (tmp > 0xffff)
		bad_id(id);
	product = tmp;
}
