/*
 * idbg-reset/idbg-reset.c - Reset utility for target and IDBG
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
#include <string.h>
#include <usb.h>

#include "idbg/usb-ids.h"
#include "idbg/ep0.h"
#include "../lib/usb.h"


#define TO_DEV		0x40
#define	FROM_DEV	0xc0


#define	nRESET_GTA	0x100
#define	nRESET_BEN	0x4000


static void reset_target(usb_dev_handle *dev)
{
	int res;

	res = usb_control_msg(dev, TO_DEV, IDBG_GPIO_DATA_SET,
	    0, nRESET_GTA | nRESET_BEN, NULL, 0, 1000);
	if (res < 0) {
		fprintf(stderr, "IDBG_GPIO_DATA_SET: %d\n", res);
		exit(1);
	}
	res = usb_control_msg(dev, TO_DEV, IDBG_GPIO_DATA_SET,
	    nRESET_GTA | nRESET_BEN, nRESET_GTA | nRESET_BEN, NULL, 0, 1000);
	if (res < 0) {
		fprintf(stderr, "IDBG_GPIO_DATA_SET: %d\n", res);
		exit(1);
	}
}


static void reset_idbg(usb_dev_handle *dev)
{
	int res;

	fprintf(stderr, "(expect an error because IDBG will not ACK)\n");
	res = usb_control_msg(dev, TO_DEV, IDBG_RESET, 0, 0, NULL, 0, 1000);
	if (res < 0) {
		fprintf(stderr, "IDBG_RESET: %d\n", res);
		exit(1);
	}
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-d|-t]\n\n"
"  -d  reset IDBG\n"
"  -t  reset target (default)\n"
    , name);
	exit(1);
}


int main(int argc, const char **argv)
{
	usb_dev_handle *dev;
	int target = 1;

	switch (argc) {
	case 1:
		break;
	case 2:
		if (!strcmp(argv[1], "-t"))
			break;
		target = 0;
		if (!strcmp(argv[1], "-d"))
			break;
		/* fall through */
	default:
		usage(*argv);
	}

	dev = open_usb();
	if (!dev) {
		fprintf(stderr, ":-(\n");
		exit(1);
	}
	if (target)
		reset_target(dev);
	else
		reset_idbg(dev);
	return 0;
}
