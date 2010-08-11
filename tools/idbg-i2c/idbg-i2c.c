/*
 * idbg-i2c/idbg-i2c.c - Read/write I2C registers via IDBG
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
#include <unistd.h>
#include <string.h>
#include <usb.h>

#include "idbg/usb-ids.h"
#include "idbg/ep0.h"
#include "../lib/usb.h"
#include "../lib/identify.h"


#define TO_DEV		0x40
#define	FROM_DEV	0xc0


static void i2c_fetch(usb_dev_handle *dev, ssize_t size)
{
	uint8_t buf[10];
	ssize_t left = size, res;
	int i;

	while (left) {
		res = usb_control_msg(dev, FROM_DEV, IDBG_I2C_FETCH, 0, 0,
		    (void *) buf, sizeof(buf), 1000);
		if (res < 0) {
			fprintf(stderr, "IDBG_I2C_FETCH: %d\n", (int) res);
			exit(1);
		}
		for (i = 0; i != res; i++)
			printf("%02X ", buf[i]);
		left -= res;
		if (left)
			usleep(10000);
	}
	putchar('\n');
}


static void i2c_write(usb_dev_handle *dev, uint8_t slave, uint8_t addr,
    uint8_t *buf, size_t size)
{
	int res;

	res = usb_control_msg(dev, TO_DEV, IDBG_I2C_WRITE,
	    addr, slave, (void *) buf, size, 1000);
	if (res < 0) {
		fprintf(stderr, "IDBG_I2C_WRITE: %d\n", res);
		exit(1);
	}
	i2c_fetch(dev, size);
}


static void i2c_read(usb_dev_handle *dev, uint8_t slave, uint8_t addr,
    uint8_t *buf, size_t size)
{
	int res;

	res = usb_control_msg(dev, FROM_DEV, IDBG_I2C_READ,
	    addr, slave, (void *) buf, size, 1000);
	if (res < 0) {
		fprintf(stderr, "IDBG_I2C_READ: %d\n", res);
		exit(1);
	}
	i2c_fetch(dev, size);
}


static void usage(const char *name)
{
	fprintf(stderr,
	    "usage: %s [-d vendor:product] slave address [value]\n", name);
	exit(1);
}


int main(int argc, char **argv)
{
	usb_dev_handle *dev;
	int c, hw;
	int set = 0;
	unsigned long slave, addr, value;
	uint8_t tmp;
	char *end;

	while ((c = getopt(argc, argv, "d:")) != EOF)
		switch (c) {
		case 'd':
			parse_usb_id(optarg);
			break;
		default:
			usage(*argv);
		}

	switch (argc-optind) {
	case 3:
		set = 1;
		value = strtoul(argv[optind+2], &end, 0);
		if (*end || value > 0xff)
			usage(*argv);
		/* fall through */
	case 2:
		slave = strtoul(argv[optind], &end, 0);
		if (*end || slave > 0x7f)
			usage(*argv);
		addr = strtoul(argv[optind+1], &end, 0);
		if (*end || addr > 0xff)
			usage(*argv);
		break;
	default:
		usage(*argv);
	}

	dev = open_usb();
	if (!dev) {
		fprintf(stderr, ":-(\n");
		exit(1);
	}

	hw = idbg_get_hw_type(dev);
	if (hw != HW_TYPE_GTA) {
		fprintf(stderr, "IDBG connects to I2C only on GTA "
		"(hw revision %d), not on revision %d\n",
		HW_TYPE_GTA, hw);
		exit(1);
	}

	if (!set)
		i2c_read(dev, slave, addr, &tmp, 1);
	else {
		tmp = value;
		i2c_write(dev, slave, addr, &tmp, 1);
	}

	return 0;
}
