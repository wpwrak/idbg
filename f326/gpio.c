/*
 * f326/gpio.c - Really primitive S3C244x GPIO access. Ports B-H only.
 *
 * Written 2008 by Werner Almesberger
 * Copyright 2008 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>


#define BASE 0x56000000


volatile void *mem;


void gpio_init(void)
{
	int fd;

	fd = open("/dev/mem", O_RDWR);
        if (fd < 0) {
		perror("/dev/mem");
		exit(1);
	}
	mem = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE);
	if (mem == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
}
