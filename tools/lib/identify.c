/*
 * lib/identify.c - Identify an IDBG board
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>
#include <stdio.h>
#include <usb.h>

#include "idbg/usb-ids.h"
#include "idbg/ep0.h"


#define FROM_DEV	0xc0


static int get_id(usb_dev_handle *dev, void *data, int size)
{
	int res;

	res = usb_control_msg(dev, FROM_DEV, IDBG_ID, 0, 0, data, size, 1000);
	if (res < 0)
		fprintf(stderr, "IDBG_ID: %s\n", usb_strerror());
	return res;
}


int idbg_get_protocol(usb_dev_handle *dev, uint8_t *major, uint8_t *minor)
{
	uint8_t ids[2];

	if (get_id(dev, ids, 2) < 0)
		return -1;
	if (major)
		*major = ids[0];
	if (minor)
		*minor = ids[1];

	return 0;
}


int idbg_get_hw_type(usb_dev_handle *dev)
{
	uint8_t major, minor;
	uint8_t ids[3];

	if (idbg_get_protocol(dev, &major, &minor) < 0)
		return -1;
	if (!major && !minor)
		return HW_TYPE_GTA;
	if (get_id(dev, ids, 3) < 0)
		return -1;
	return ids[2];
}


int idbg_get_build_number(usb_dev_handle *dev, uint16_t *build)
{
	uint8_t major, minor;
	int res;

	if (idbg_get_protocol(dev, &major, &minor) < 0)
		return -1;
	if (!major && minor < 2)
		return 0;
	res = usb_control_msg(dev, FROM_DEV, IDBG_BUILD_NUMBER, 0, 0,
	    (void *) build, 2, 1000);
	if (res < 0)
		fprintf(stderr, "IDBG_BUILD_NUMBER: %s\n", usb_strerror());
	return res;
}


int idbg_get_build_date(usb_dev_handle *dev, char *buf, size_t size)
{
	uint8_t major, minor;
	int res;

	if (idbg_get_protocol(dev, &major, &minor) < 0)
		return -1;
	if (!major && minor < 2) {
		if (size)
			*buf = 0;
		return 0;
	}
	res = usb_control_msg(dev, FROM_DEV, IDBG_BUILD_DATE, 0, 0, buf, size,
	    1000);
	if (res < 0)
		fprintf(stderr, "IDBG_BUILD_DATE: %s\n", usb_strerror());
	return res;
}


int idbg_print_id(FILE *file, usb_dev_handle *dev)
{
	const struct usb_device *device = usb_device(dev);
	const char *vendor, *product, *hw_type;
	uint8_t major, minor;
	int type = HW_TYPE_GTA;
	int res;

	switch (device->descriptor.idVendor) {
	case USB_VENDOR_OPENMOKO:
		vendor = "Openmoko Inc.";
		break;
	case USB_VENDOR_QI_HW:
		vendor = "Qi Hardware";
		break;
	default:
		vendor = "???";
		break;
	}

	switch (device->descriptor.idProduct) {
	case USB_PRODUCT_IDBG:
		product = "IDBG";
		break;
	default:
		product = "???";
		break;
	}

	if (idbg_get_protocol(dev, &major, &minor) < 0)
		return -1;
	if (major || minor) {
		type = idbg_get_hw_type(dev);
		if (type < 0)
			return -1;
	}

	switch (type) {
	case HW_TYPE_GTA:
		hw_type = "GTA01/02";
		break;
	case HW_TYPE_BEN_V1:
		hw_type = "Ben, IDBG V1";
		break;
	case HW_TYPE_BEN_V2:
		hw_type = "Ben, IDBG V2";
		break;
	default:
		hw_type = "???";
		break;
	}
	
	res = fprintf(file,
	    "%04x:%04x (%s, %s) protocol %d.%d hw type %d (%s)",
	    device->descriptor.idVendor, device->descriptor.idProduct,
	    vendor, product, major, minor, type, hw_type);
	if (res < 0)
		perror("fprintf");
	return res;
}
