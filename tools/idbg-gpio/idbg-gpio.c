/*
 * idbg-gpio/idbg-gpio.c - GPIO get/set utility
 *
 * Written 2008 by Werner Almesberger
 * Copyright 2008 Werner Almesberger
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
#include <strings.h>
#include <usb.h>

#include "ep0idbg.h"


#define TO_DEV		0x40
#define	FROM_DEV	0xc0


/*
 * @@@ we really ought to generate this from a central table
 */

static struct pin {
	int pin;
	int port, bit;
	const char *neo_name;
	const char *idbg_name;
} pins[] = {
	{  1,	0, 0,	NULL,		"FUTURE"	},
	{ 11,	2, 3,	"GSM_EN",	NULL		},
	{ 12,	2, 2,	"nNOR_WP",	NULL		},
	{ 16,	2, 5,	"STDI",		"TDI"		},
	{ 17,	2, 4,	"STMS",		"TMS"		},
	{ 18,	2, 1,	"STDO",		"TDO"		},
	{ 19,	2, 0,	"nSRST",	"nRESET"	},
	{ 22,	0, 7,	"STCK",		"TCK"		},
	{ 23,	0, 6,	"nTRST",	NULL		},
	{ 24,	0, 5,	"CONSOLE_TXD",	"RX"		},
	{ 25,	0, 4,	"CONSOLE_RXD",	"TX"		},
	{ 26,	0, 3,	"I2C_SCL",	"SCL",		},
	{ 27,	0, 2,	"I2C_SDA",	"SDA",		},
	{ 28,	0, 1,	NULL,		"SDA_PULL",	},
	{ 0, }
};


static uint8_t data[2], mode[2], mask[2];
static int have_get = 0;


static usb_dev_handle *open_usb(void)
{
	const struct usb_bus *bus;
	struct usb_device *dev;

	usb_init();
	usb_find_busses();
	usb_find_devices();

	for (bus = usb_get_busses(); bus; bus = bus->next)
		for (dev = bus->devices; dev; dev = dev->next) {
			if (dev->descriptor.idVendor != 0x1234)
				continue;
			if (dev->descriptor.idProduct != 0x0002)
				continue;
			return usb_open(dev);
		}
	return NULL;
}


static const struct pin *lookup_pin(const char *name)
{
	const struct pin *pin;
	char tmp[5];

	for (pin = pins; pin->pin; pin++) {
		if (pin->neo_name && !strcasecmp(pin->neo_name, name))
			return pin;
		if (pin->idbg_name && !strcasecmp(pin->idbg_name, name))
			return pin;
		sprintf(tmp, "%d", pin->pin);
		if (!strcmp(tmp, name))
			return pin;
		sprintf(tmp, "P%d_%d", pin->port, pin->bit);
		if (!strcasecmp(tmp, name))
			return pin;
	}
	return NULL;
}


static void set_pin(const struct pin *pin, const char *value)
{
	int port = !!pin->port;
	int bit = 1 << pin->bit;

	mask[port] |= bit;
	if (!*value || value[1]) {
		fprintf(stderr, "invalid pin state \"%s\"\n", value);
		exit(1);
	}
	switch (*value) {
	case '1':
		mode[port] |= bit;
		/* fall through */
	case 'r':
	case 'R':
		data[port] |= bit;
		break;
	case '0':
		break;
	default:
		fprintf(stderr, "invalid pin state \"%s\"\n", value);
		exit(1);
	}
}


static void parse_set(const char **args, int num_args)
{
	const struct pin *pin;
	const char *eq;
	char tmp[100]; /* enough :-) */

	while (num_args--) {
		eq = strchr(*args, '=');
		if (!eq) {
			have_get = 1;
		} else {
			memcpy(tmp, *args, eq-*args);
			tmp[eq-*args] = 0;
			pin = lookup_pin(tmp);
			if (!pin) {
				fprintf(stderr, "unrecognized pin \"%s\"\n",
				    tmp);
				exit(1);
			}
			set_pin(pin, eq+1);
		}
		args++;
	}
}


static void print_names(const struct pin *pin)
{
	printf("%2d: P%d_%d (", pin->pin, pin->port, pin->bit);
	if (pin->neo_name)
		printf("%s", pin->neo_name);
	if (pin->neo_name && pin->idbg_name)
		printf(", ");
	if (pin->idbg_name)
		printf("%s", pin->idbg_name);
	printf(")");
}


static void get_pin(const struct pin *pin)
{
	int port = !!pin->port;
	int bit = 1 << pin->bit;

	print_names(pin);
	printf(" = %c\n",
	    data[port] & bit ? mode[port] & bit ? '1' : 'R' : '0');
}


static void parse_get(const char **args, int num_args)
{
	const struct pin *pin;

	while (num_args--) {
		if (!strchr(*args, '=')) {
			pin = lookup_pin(*args);
			if (!pin) {
				fprintf(stderr, "unrecognized pin \"%s\"\n",
				    *args);
				exit(1);
			}
			get_pin(pin);
		}
		args++;
	}
}


static void list_pins(void)
{
	const struct pin *pin;

	for (pin = pins; pin->pin; pin++) {
		print_names(pin);
		putchar('\n');
	}
}


static void dump_all(void)
{
	const struct pin *pin;

	for (pin = pins; pin->pin; pin++)
		get_pin(pin);
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-a] pin[=value] ...\n"
"       %s -l\n\n"
"  -a  dump the state of all pins\n"
"  -l  list valid pin names\n\n"
"Pin values are 0, 1, and R.\n"
  , name, name);
	exit(1);
}


int main(int argc, const char **argv)
{
	usb_dev_handle *dev;
	int res, all = 0;

	if (argc == 2 && !strcmp(argv[1], "-l")) {
		list_pins();
		return 0;
	}

	dev = open_usb();
	if (!dev) {
		fprintf(stderr, ":-(\n");
		exit(1);
	}
	res = usb_reset(dev);
	if (res < 0) {
		fprintf(stderr, "usb_reset: %d\n", res);
		return 1;
	}

	if (argc > 1) {
		if (!strcmp(argv[1], "-a"))
			all = 1;
		else if (*argv[1] == '-')
			usage(*argv);
	}

	parse_set(argv+1, argc-1);
	if (mask[0] || mask[1]) {
		res = usb_control_msg(dev, TO_DEV, IDBG_GPIO_DATA_SET,
		    data[0] | data[1] << 8, mask[0] | mask[1] << 8,
		    NULL, 0, 1000);
		if (res < 0) {
			fprintf(stderr, "IDBG_GPIO_DATA_SET: %d\n", res);
			return 1;
		}
		res = usb_control_msg(dev, TO_DEV, IDBG_GPIO_MODE_SET,
		    mode[0] | mode[1] << 8, mask[0] | mask[1] << 8,
		    NULL, 0, 1000);
		if (res < 0) {
			fprintf(stderr, "IDBG_GPIO_MODE_SET: %d\n", res);
			return 1;
		}
	}

	if (all || have_get) {
		res = usb_control_msg(dev, FROM_DEV, IDBG_GPIO_DATA_GET, 0, 0,
		    (void *) data, 2, 1000);
		if (res < 0) {
			fprintf(stderr, "IDBG_GPIO_DATA_GET: %d\n", res);
			return 1;
		}
		res = usb_control_msg(dev, FROM_DEV, IDBG_GPIO_MODE_GET, 0, 0,
		    (void *) mode, 2, 1000);
		if (res < 0) {
			fprintf(stderr, "IDBG_GPIO_MODE_GET: %d\n", res);
			return 1;
		}
		if (all)
			dump_all();
		else
			parse_get(argv+1, argc-1);
	}

	return 0;
}
