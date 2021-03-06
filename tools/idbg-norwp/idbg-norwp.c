/*
 * idbg-norwp/idbg-norwp.c - Control the GTA02 nNOR_WP line through IDBG
 *
 * Written 2008-2010 by Werner Almesberger
 * Copyright (C) 2008 by OpenMoko, Inc.
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
#include <string.h>
#include <usb.h>

#include "idbg/usb-ids.h"
#include "idbg/ep0.h"
#include "../lib/usb.h"
#include "../lib/identify.h"


#define TO_DEV		0x40
#define	FROM_DEV	0xc0


#define	nNOR_WP	0x400


static void norwp_query(usb_dev_handle *dev)
{
	uint8_t data[2];
	int res;

	res = usb_control_msg(dev, FROM_DEV, IDBG_GPIO_DATA_GET, 0, 0,
	    (void *) data, 2, 1000);
	if (res < 0) {
		fprintf(stderr, "IDBG_GPIO_DATA_GET: %d\n", res);
		exit(1);
	}
	printf("%s\n", data[1] & (nNOR_WP >> 8) ? "rw" : "ro");
}


static void norwp_set(usb_dev_handle *dev, int high)
{
	int res;

	res = usb_control_msg(dev, TO_DEV, IDBG_GPIO_DATA_SET,
	    high ? nNOR_WP : 0, nNOR_WP, NULL, 0, 1000);
	if (res < 0) {
		fprintf(stderr, "IDBG_GPIO_DATA_SET: %d\n", res);
		exit(1);
	}
	res = usb_control_msg(dev, TO_DEV, IDBG_GPIO_MODE_SET,
	    nNOR_WP, nNOR_WP, NULL, 0, 1000);
	if (res < 0) {
		fprintf(stderr, "IDBG_GPIO_MODE_SET: %d\n", res);
		exit(1);
	}
}


static void usage(const char *name)
{
	fprintf(stderr, "usage; %s [ro|rw]\n", name);
	exit(1);
}


int main(int argc, char **argv)
{
	usb_dev_handle *dev;
	int set = 0, rw = 0;
	int hw;

	if (argc != 1 && argc != 2)
		usage(*argv);
	if (argc == 2) {
		if (!strcmp(argv[1], "ro"))
			set = 1;
		else if (!strcmp(argv[1], "rw"))
			set = rw = 1;
		else
			usage(*argv);
	}

	dev = open_usb();
	if (!dev) {
		fprintf(stderr, ":-(\n");
		exit(1);
	}

	hw = idbg_get_hw_type(dev);
	if (hw != HW_TYPE_GTA) {
		fprintf(stderr, "nNOR_WP only exists on IDBG for GTA02 "
		    "(hw revision %d), not on revision %d\n",
		    HW_TYPE_GTA, hw);
		exit(1);
	}

	if (set)
		norwp_set(dev, rw);
	else
		norwp_query(dev);
	return 0;
}
