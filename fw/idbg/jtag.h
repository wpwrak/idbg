/*
 * idbg/jtag.h - JTAG protocol engine
 *
 * Written 2008, 2009 by Werner Almesberger
 * Copyright 2008, 2009 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef JTAG_H
#define JTAG_H

#include <stdint.h>


#define JTAG_MAX_BITS	255


enum jtag_gpios {
	JTAG_nTRST,
	JTAG_TMS,
	JTAG_TDO,
	JTAG_TDI,
	JTAG_TCK,
	JTAG_nSRST,
};


extern __xdata uint8_t jtag_data[];

uint8_t jtag_gpio_get(void);
void jtag_gpio_set(uint8_t set, uint8_t mask);

void jtag_scan(const uint8_t *buf, uint16_t bits, __bit last);
void jtag_move(const uint8_t *buf, uint16_t bits);

void jtag_attach(void);
void jtag_detach(void);

#endif /* !JTAG_H */
