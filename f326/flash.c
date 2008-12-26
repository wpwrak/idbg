/*
 * f326/flash.c - Flash programming and reading
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
#include <unistd.h>

#include "c2.h"
#include "flash.h"


/* ----- Helper functions for common flash protocol idioms ----------------- */


static uint8_t _c2_addr_read(void)
{
    uint8_t x;

    usleep(1000);
    x = c2_addr_read();
//    fprintf(stderr, "got 0x%02x\n", x);
    return x;
}


static void wait_busy(void)
{
    while (_c2_addr_read() & InBusy);
}


static void wait_ready(void)
{
    while (!(_c2_addr_read() & OutReady));
}


static void fpdat_write(uint8_t value)
{
    c2_data_write(value, 1);
    wait_busy();
}


static uint8_t fpdat_read(void)
{
    wait_ready();
    return c2_data_read(1);
}


static void check_status(void)
{
    uint8_t status;

    status = fpdat_read();
    if (status != FLASH_STATUS_OK) {
	fprintf(stderr, "status 0x%02x\n", status);
	exit(1);
    }
}


/* ----- Block/device-level flash operations ------------------------------- */


void flash_device_erase(void)
{
    c2_addr_write(FPDAT);
    fpdat_write(FLASH_DEVICE_ERASE);
    check_status();
    fpdat_write(FLASH_ERASE_MAGIC1);
    fpdat_write(FLASH_ERASE_MAGIC2);
    fpdat_write(FLASH_ERASE_MAGIC3);
    check_status();
}


void flash_block_write(uint16_t addr, const void *data, size_t size)
{
    int i;

    if (!size)
	return;
    c2_addr_write(FPDAT);
    fpdat_write(FLASH_BLOCK_WRITE);
    check_status();
    fpdat_write(addr >> 8);
    fpdat_write(addr);
    fpdat_write(size);
    check_status();
    for (i = 0; i != size; i++)
	fpdat_write(((uint8_t *) data)[i]);
}


void flash_block_read(uint16_t addr, void *data, size_t size)
{
    int i;

    if (!size)
	return;
    c2_addr_write(FPDAT);
    fpdat_write(FLASH_BLOCK_READ);
    check_status();
    fpdat_write(addr >> 8);
    fpdat_write(addr);
    fpdat_write(size);
    check_status();
    for (i = 0; i != size; i++)
	((uint8_t *) data)[i] = fpdat_read();
}


void flash_init(void)
{
    c2_addr_write(FPCTL);
    c2_data_write(FLASH_INIT_MAGIC1, 1);
    c2_data_write(FLASH_INIT_MAGIC2, 1);
    usleep(20000);
}


/* @@@ doesn't really seem to work */

uint8_t fp_reg_read(uint8_t addr)
{
    c2_addr_write(FPDAT);
    fpdat_write(REG_READ);
    check_status();
    fpdat_write(addr);
    fpdat_write(1);
    return fpdat_read();
}


void fp_reg_write(uint8_t addr, uint8_t value)
{
    c2_addr_write(FPDAT);
    fpdat_write(REG_WRITE);
    check_status();
    fpdat_write(addr);
    fpdat_write(1);
    fpdat_write(value);
}

