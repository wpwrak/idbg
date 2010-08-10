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


static void usage(const char *name)
{
	fprintf(stderr, "%s\n", name);
	exit(1);
}


int main(int argc, const char **argv)
{
	usb_dev_handle *dev;

	if (argc != 1)
		usage(*argv);
	dev = open_usb();
	if (!dev) {
		fprintf(stderr, ":-(\n");
		exit(1);
	}

	if (idbg_print_id(stdout, dev) < 0)
		return 1;

	printf("\n");
	fflush(stdout);

	return 0;
}
