/*
 * idbg-gpio/idbg-gpio.c - GPIO get/set utility
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
#include <strings.h>
#include <usb.h>

#include "idbg/usb-ids.h"
#include "idbg/ep0.h"
#include "../lib/usb.h"
#include "../lib/identify.h"


#define TO_DEV		0x40
#define	FROM_DEV	0xc0


struct pin {
	int pin;	/* on C8051F326 */
	int port, bit;
	const char *host_name;
	const char *idbg_name;
};


/*
 * @@@ we really ought to generate this from a central table
 */

static struct pin gta_pins[] = {
	{  1,	0, 0,	"I2C_SCL",	"SCL",		},
	{ 11,	2, 3,	"nGSM_EN",	NULL		},
	{ 12,	2, 2,	"nNOR_WP",	NULL		},	
	{ 16,	2, 5,	"STDI",		"TDI"		},
	{ 17,	2, 4,	"STMS",		"TMS"		},
	{ 18,	2, 1,	"STDO",		"TDO"		},
	{ 19,	2, 0,	"nSRST",	"nRESET"	},
	{ 22,	0, 7,	"STCK",		"TCK"		},
	{ 23,	0, 6,	"nTRST",	NULL		},
	{ 24,	0, 5,	"CONSOLE_TXD",	"RX"		},
	{ 25,	0, 4,	"CONSOLE_RXD",	"TX"		},
	{ 26,	0, 3,	NULL,		"FUTURE"	},
	{ 27,	0, 2,	"I2C_SDA",	"SDA",		},
	{ 28,	0, 1,	NULL,		"SDA_PULL",	},
	{ 0, }
};


static struct pin ben_v1_pins[] = {
	{ 24,	0, 5,	"TDO",		"RX"		},
	{ 25,	0, 4,	"TDI",		"TX"		},
	{ 26,	0, 3,	"BOOT_SEL1",	"USB_BOOT",	},
	{ 10,	3, 0,	"RESET_N",	NULL		},
	{ 0, }
};


static struct pin ben_v2_pins[] = {
	{ 24,	0, 5,	"TDO",		"RX"		},
	{ 25,	0, 4,	"TDI",		"TX"		},
	{ 28,	0, 1,	"RESET_N",	NULL		},
	{ 10,	3, 0,	"BOOT_SEL1",	"USB_BOOT",	},
	{ 0, }
};


static uint8_t data[2], mode[2], mask[2];
static int have_get = 0;
static const struct pin *pins = NULL; /* set after identifying board */


static const struct pin *lookup_pin(const char *name)
{
	const struct pin *pin;
	char tmp[5];

	for (pin = pins; pin->pin; pin++) {
		if (pin->host_name && !strcasecmp(pin->host_name, name))
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


static void translate_pin(const struct pin *pin, int *port, int *bit)
{
	if (pin->port != 3) {
		*port = !!pin->port;
		*bit = 1 << pin->bit;
	} else {
		*port = 1;
		*bit = 1 << 6;
	}
}


static void set_pin(const struct pin *pin, const char *value)
{
	int port, bit;

	translate_pin(pin, &port, &bit);
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
	if (pin->host_name)
		printf("%s", pin->host_name);
	if (pin->host_name && pin->idbg_name)
		printf(", ");
	if (pin->idbg_name)
		printf("%s", pin->idbg_name);
	printf(")");
}


static void get_pin(const struct pin *pin)
{
	int port, bit;

	translate_pin(pin, &port, &bit);
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


static void dump_pin_table(const char *device, const struct pin *pin_table)
{
	const struct pin *pin;

	printf("%s\n", device);
	for (pin = pin_table; pin->pin; pin++) {
		printf("  ");
		print_names(pin);
		putchar('\n');
	}
}


static void list_pins(void)
{
	dump_pin_table("GTA01/GTA02", gta_pins);
	dump_pin_table("Ben NanoNote, IDBG V1", ben_v1_pins);
	dump_pin_table("Ben NanoNote, IDBG V2", ben_v2_pins);
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
"usage: %s\n"
"       %s pin[=value] ...\n"
"       %s -l\n\n"
"  -l  list valid pin names\n\n"
"Pin values are 0, 1, and R.\n"
  , name, name, name);
	exit(1);
}


int main(int argc, const char **argv)
{
	usb_dev_handle *dev;
	int res, all = 0;
	int hw;

	if (argc == 2 && !strcmp(argv[1], "-l")) {
		list_pins();
		return 0;
	}

	dev = open_usb();
	if (!dev) {
		fprintf(stderr, ":-(\n");
		exit(1);
	}

	hw = idbg_get_hw_type(dev);
	switch (hw) {
	case HW_TYPE_GTA:
		pins = gta_pins;
		break;
	case HW_TYPE_BEN_V1:
		pins = ben_v1_pins;
		break;
	case HW_TYPE_BEN_V2:
		pins = ben_v2_pins;
		break;
	default:
		fprintf(stderr, "unknown hardware revision %d\n", hw);
		exit(1);
	}

	if (argc == 1)
		all = 1;
	else {
		if (*argv[1] == '-')
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
