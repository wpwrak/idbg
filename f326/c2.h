/*
 * f326/c2.h - Basic C2 messages
 *
 * Written 2008 by Werner Almesberger
 * Copyright 2008 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef C2_H
#define C2_H


#include <stdint.h>


#define C2_DATA_READ	0
#define C2_DATA_WRITE	1
#define C2_ADDR_READ	2
#define C2_ADDR_WRITE	3


void c2_addr_write(uint8_t addr);
uint8_t c2_addr_read(void);
void c2_data_write(uint32_t data, int bytes);
uint32_t c2_data_read(int bytes) ;

void c2_init(void);
void c2_reset(void);

#endif /* !C2_H */
