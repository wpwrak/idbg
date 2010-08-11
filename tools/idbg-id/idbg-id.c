/*
 * idbg-id/idbg-id.c - Identify an IDBG
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdlib.h>
#include <stdio.h>

#include "../lib/usb.h"
#include "../lib/identify.h"


#define	BUF_SIZE	256


static void usage(const char *name)
{
	fprintf(stderr, "%s\n", name);
	exit(1);
}


int main(int argc, const char **argv)
{
	usb_dev_handle *dev;
	uint16_t build;
	char buf[BUF_SIZE+1];	/* +1 for terminating \0 */
	int len;

	if (argc != 1)
		usage(*argv);
	dev = open_usb();
	if (!dev) {
		fprintf(stderr, ":-(\n");
		return 1;
	}

	if (idbg_print_id(stdout, dev) < 0)
		return 1;

	printf("\n");

	len = idbg_get_build_number(dev, &build);
	if (len < 0)
		return 1;
	switch (len) {
	case 0:
		build = 0; /* we'll probably fail idbg_get_build_date */
		break;
	case 2:
		break;
	default:
		fprintf(stderr, "build number length (%d) != 2\n", len);
		return 1;
	}
	len = idbg_get_build_date(dev, buf, BUF_SIZE);
	if (len < 0)
		return 1;
	if (len) {
		buf[len] = 0;
		printf("%10s%s #%u\n", "", buf, (unsigned) build);
	}

	return 0;
}
