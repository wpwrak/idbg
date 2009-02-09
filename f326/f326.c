/*
 * f326/f326.c - Simple C8051F326/7 Flash programmer
 *
 * Written 2008, 2009 by Werner Almesberger
 * Copyright 2008, 2009 Werner Almesberger
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
#include <sys/types.h>

#include "c2.h"
#include "flash.h"
#include "boundary.h"


#define	LOCK_BYTE	0x3dff


static size_t file_size;


static void dump(const char *title, void *data, size_t size)
{
    int i, j;

    fprintf(stderr, "%s:\n", title);
    for (i = 0; i < size; i += 16) {
	fprintf(stderr, "  %04x", i);
	for (j = 0; j != 16 && i+j < size; j++)
	    fprintf(stderr, " %02x", ((uint8_t *) data)[i+j]);
	fprintf(stderr, "\n");
    }
}


static void flash_device(void *data, size_t size)
{
    int i;
    size_t len;
    uint8_t buf[256];

    for (i = 0; i < size; i += 256)
	fputc('-', stderr);
    fputc('\r', stderr);

    flash_init();
    flash_device_erase();

    for (i = 0; i < size; i += 256) {
	fputc('*', stderr);
	fflush(stderr);
	len = size-i <= 256 ? size-i : 256;
	flash_block_write(i, data+i, len);
    }
    fputc('\r', stderr);

    for (i = 0; i < size; i += 256) {
	fputc('#', stderr);
	fflush(stderr);
	len = size-i <= 256 ? size-i : 256;
	flash_block_read(i, buf, len);
	if (memcmp(buf, data+i, len)) {
	    fprintf(stderr, "compare error at 0x%04x\n", i);
	    dump("Expected", data+i, len);
	    dump("Read", buf, len);
	    exit(1);
	}
    }
    fputc('\n', stderr);
}


static void erase_flash(void)
{
    flash_init();
    flash_device_erase();
}


static void dump_flash(size_t size)
{
    int i, j;
    size_t len;
    uint8_t buf[256], last[256];
    int skipping = 0;

    flash_init();
    for (i = 0; i < size; i += 16) {
	len = size-i <= 16 ? size-i : 16;
	flash_block_read(i, buf, len);
	if (i && !memcmp(last, buf, len)) {
	    printf("%04x: *%c", i, skipping ? '\r' : '\n');
	    fflush(stdout);
	    skipping = 1;
	    continue;
	}
	skipping = 0;
	memcpy(last, buf, len);
	printf("%04x:", i);
	for (j = 0; j != len; j++)
	    printf(" %02x", buf[j]);
	printf("  ");
	for (j = 0; j != len; j++)
	    printf("%c", buf[j] >= ' ' && buf[j] <= '~' ? buf[j] : '.');
	putchar('\n');
	fflush(stdout);
    }
}


static void identify(void)
{
    int i;

    c2_addr_write(0);
    printf("Dev");
    for (i = 0; i != 10; i++)
	printf(" 0x%02x", c2_data_read(1));
    c2_addr_write(1);
    printf("\nRev");
    for (i = 0; i != 10; i++)
	printf(" 0x%02x", c2_data_read(1));
    printf("\n");
}


static void do_flash(const char *name)
{
    FILE *file;
    uint8_t code[16384];

    file = fopen(name, "r");
    if (!file) {
	perror(name);
	exit(1);
    }
    file_size = fread(code, 1, sizeof(code), file);
    (void) fclose(file);
    flash_device(code, file_size);
}


static void protect(void)
{
    uint8_t pages, lock_byte;

    pages = (file_size+511) >> 9;
    printf("Protecting %d page%s\n", pages, pages == 1 ? "" : "s");
    lock_byte = ~pages;
    flash_block_write(LOCK_BYTE, &lock_byte, 1);
}


static void usage(const char *name)
{
    fprintf(stderr,
"usage: %s [-p] file\n"
"       %s -d\n"
"       %s -e\n"
"       %s -b pin_setup\n"
"       %s\n\n"
"  -b pin_setup\n"
"    Perform a boundary scan. pin_setup sets all 14 pins in this order:\n"
"    P0_0, P0_1, ..., P0_7, P2_0, ..., P2_5.\n"
"    Pins can be set to 0, 1, or R (pull-up). Dots can be used to structure\n"
"    the bit string. Prints what the pins read back (0 or 1) in the same\n"
"    order, with a dot between P0 and P2.\n"
"  -d  dump Flash content\n"
"  -e  erase whole Flash\n"
"  -p  Protect the data after writing\n"
"Invocation without argument resets the F326.\n"
  , name, name, name, name, name);
    exit(1);
}


int main(int argc, char **argv)
{
    c2_init();
    identify();

    switch (argc) {
    case 1:
	/* just reset */
	break;
    case 2:
	if (!strcmp(argv[1], "-d"))
	    dump_flash(0x4000);
	else if (!strcmp(argv[1], "-e"))
	    erase_flash();
	else if (*argv[1] == '-')
	    usage(*argv);
	else {
	    do_flash(argv[1]);
	    identify();
	}
	break;
    case 3:
	if (!strcmp(argv[1], "-p")) {
	    if (*argv[2] == '-')
		usage(*argv);
	    do_flash(argv[2]);
	    protect();
	    identify();
	    break;
	}
	if (strcmp(argv[1], "-b"))
	    usage(*argv);
	boundary(argv[2]);
	break;
    default:
	usage(*argv);
    }
    c2_reset();

    return 0;
}
