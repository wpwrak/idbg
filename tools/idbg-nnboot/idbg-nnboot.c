/*
 * idbg-nnboot/idbg-nnboot.c - Control the NanoNote boot source through IDBG
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

#define	BOOT_SEL1_BEN_V1	0x0008	/* P0_3 */
#define	BOOT_SEL1_BEN_V2	0x4000	/* P3_0, folded into P2 */


enum boot_source {
	bs_nand,
	bs_usb,
};


static void nnboot_query(usb_dev_handle *dev, uint16_t mask)
{
	uint8_t data[2];
	uint16_t value;
	int res;

	res = usb_control_msg(dev, FROM_DEV, IDBG_GPIO_DATA_GET, 0, 0,
	    (void *) data, 2, 1000);
	if (res < 0) {
		fprintf(stderr, "IDBG_GPIO_DATA_GET: %d\n", res);
		exit(1);
	}
	value = data[0] | data[1] << 8;
	printf("%s\n", value & mask ? "nand" : "usb");
}


static void nnboot_set(usb_dev_handle *dev, uint16_t mask, int high)
{
	int res;

	res = usb_control_msg(dev, TO_DEV, IDBG_GPIO_DATA_SET,
	    high ? mask : 0, mask, NULL, 0, 1000);
	if (res < 0) {
		fprintf(stderr, "IDBG_GPIO_DATA_SET: %d\n", res);
		exit(1);
	}
	res = usb_control_msg(dev, TO_DEV, IDBG_GPIO_MODE_SET,
	    0, mask, NULL, 0, 1000);
	if (res < 0) {
		fprintf(stderr, "IDBG_GPIO_MODE_SET: %d\n", res);
		exit(1);
	}
}


static void usage(const char *name)
{
	fprintf(stderr, "usage; %s [nand|usb]\n", name);
	exit(1);
}


int main(int argc, char **argv)
{
	usb_dev_handle *dev;
	uint16_t mask;
	enum boot_source source;
	int set = 0;
	int hw;

	if (argc != 1 && argc != 2)
		usage(*argv);
	if (argc == 2) {
		set = 1;
		if (!strcmp(argv[1], "nand"))
			source = bs_nand;
		else if (!strcmp(argv[1], "usb"))
			source = bs_usb;
		else
			usage(*argv);
	}

	dev = open_usb();
	if (!dev) {
		fprintf(stderr, ":-(\n");
		exit(1);
	}

	hw = idbg_get_hw_type(dev);
	switch (hw) {
	case HW_TYPE_BEN_V1:
		mask = BOOT_SEL1_BEN_V1;
		break;
	case HW_TYPE_BEN_V2:
		mask = BOOT_SEL1_BEN_V2;
		break;
	default:
		fprintf(stderr, "%s is for IDBG for NanoNotes "
		    "(hw revisions %d, %d), not revision %d\n", *argv,
		    HW_TYPE_BEN_V1, HW_TYPE_BEN_V2, hw);
		exit(1);
	}

	if (set)
		nnboot_set(dev, mask, source == bs_nand);
	else
		nnboot_query(dev, mask);
	return 0;
}
