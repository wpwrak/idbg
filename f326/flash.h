/*
 * f326/flash.h - Flash programming and reading
 *
 * Written 2008 by Werner Almesberger
 * Copyright 2008 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef FLASH_H
#define FLASH_H

#include <sys/types.h>


#define FPCTL	0x02
#define FPDAT	0xb4

#define FLASH_INIT_MAGIC1	0x02
#define FLASH_INIT_MAGIC2	0x01

#define FLASH_ERASE_MAGIC1	0xde
#define FLASH_ERASE_MAGIC2	0xad
#define FLASH_ERASE_MAGIC3	0xa5

#define FLASH_DEVICE_ERASE	0x03
#define	FLASH_BLOCK_READ	0x06
#define FLASH_BLOCK_WRITE	0x07
#define FLASH_PAGE_ERASE	0x08
#define	REG_READ		0x09
#define	REG_WRITE		0x0a

#define FLASH_STATUS_OK		0x0d

#define InBusy		(1 << 1)
#define	OutReady	(1 << 0)


void flash_device_erase(void);
void flash_block_write(uint16_t addr, const void *data, size_t size);
void flash_block_read(uint16_t addr, void *data, size_t size);
void flash_init(void);

/* @@@ doesn't really seem to work */

uint8_t fp_reg_read(uint8_t addr);
void fp_reg_write(uint8_t addr, uint8_t value);

#endif /* !FLASH_H */
