/*
 * idbg/jtag.c - JTAG protocol engine
 *
 * Written 2008, 2009 by Werner Almesberger
 * Copyright 2008, 2009 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "regs.h"
#include "io.h"
#include "io-parts.h"
#include "uart.h"
#include "jtag.h"


/*
 * http://hri.sourceforge.net/tools/jtag_faq_org.html
 * http://www.asset-intertech.com/pdfs/Boundary-Scan_Tutorial_2007.pdf
 */

/*
 * One extra byte so that we don't have to special-case JTAX_MAX_BITS-sized
 * scans.
 */

__xdata uint8_t jtag_data[JTAG_MAX_BITS/8+1];


uint8_t jtag_gpio_get(void)
{
	debug("jtag_gpio_get\n");
	return (nSTRST << JTAG_nTRST) |
	    (STMS << JTAG_TMS) |
	    (STDO << JTAG_TDO) |
	    (STDI << JTAG_TDI) |
	    (STCK << JTAG_TCK) |
	    (nRESET << JTAG_nSRST);
}


void jtag_gpio_set(uint8_t set, uint8_t mask)
{
	debug("jtag_gpio_set\n");
	if (mask & (1 << JTAG_nTRST))
		nSTRST = (set >> JTAG_nTRST) & 1;
	if (mask & (1 << JTAG_nSRST))
		nRESET = (set >> JTAG_nSRST) & 1;
	if (mask & (1 << JTAG_TMS))
		STMS = (set >> JTAG_TMS) & 1;
	if (mask & (1 << JTAG_TDI))
		STDI = (set >> JTAG_TDI) & 1;
	if (mask & (1 << JTAG_TCK))
		STCK = (set >> JTAG_TCK) & 1;
}


void jtag_scan(const uint8_t *buf, uint16_t bits, __bit last)
{
	uint16_t pos;
	uint8_t *res = jtag_data;
	uint8_t in = 0, out = 0;

	debug("jtag_send\n");
	STMS = 0;
	for (pos = 0; pos != bits; pos++) {
		if (!(pos & 7)) {
			if (pos)
				*res++ = in;
			in = 0;
			out = *buf++;
		}
		STCK = 0;
		STDI = out & 1;
		if (pos == bits-1 && last)
			STMS = 1;
		STCK = 1;
		out >>= 1;
		in |= STDO << (pos & 7);
	}
	*res = in;

	if (!last)
		return;

	/*
	 * Exit the shift state
	 */
	STCK = 0;
	STMS = 0;
	STDI = 0;
	STCK = 1;
}


void jtag_move(const uint8_t *buf, uint16_t bits)
{
	uint16_t pos;
	uint8_t out = 0;

	debug("jtag_move\n");
	for (pos = 0; pos != bits; pos++) {
		if (!(pos & 7))
			out = *buf++;
		STCK = 0;
		STMS = out & 1;
		STCK = 1;
		out >>= 1;
	}
}


void jtag_attach(void)
{
	debug("jtag_attach\n");

	/* Put STCK into idle state */
	STCK = 0;
	STMS = 1;

	/* Make the "fast" outputs TDI, TMS, and TCK push-pull */
	STDI_MODE |= 1 << STDI_BIT;
	STMS_MODE |= 1 << STMS_BIT;
	STCK_MODE |= 1 << STCK_BIT;
}


void jtag_detach(void)
{
	debug("jtag_detach\n");

	/* Return push-pull to open collector */
	STDI_MODE &= ~(1 << STDI_BIT);
	STCK_MODE &= ~(1 << STCK_BIT);
	STMS_MODE &= ~(1 << STMS_BIT);
	STDI = 1;
	STCK = 1;
	STMS = 1;

	/* De-assert reset */
	nRESET = 1;
	nSTRST = 1;
}
