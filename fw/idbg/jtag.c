/*
 * idbg/jtag.c - JTAG protocol engine
 *
 * Written 2008 by Werner Almesberger
 * Copyright 2008 Werner Almesberger
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

__xdata uint8_t jtag_data[JTAG_MAX_BITS/8];


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


void jtag_send(const uint8_t *buf, uint8_t bits)
{
	uint8_t *res = jtag_data;
	uint8_t in = 0, out, i;

	printk("jtag_send\n");
	while (bits > 7) {
		out = *buf++;
		for (i = 0; i != 8; i++) {
			STDI = out & 1;
			STCK = 1;
			out >>= 1;
			STCK = 0;
			in = STDO | in << 1;
		}
		*res = in;
		bits -= 8;
	}
	if (bits) {
		out = *buf;
		while (bits) {
			STDI = out & 1;
			STCK = 1;
			out >>= 1;
			STCK = 0;
			in = STDO | in << 1;
			bits--;
		}
		*res = in;
	}
}


void jtag_attach(void)
{
	debug("jtag_attach\n");

	/* Put STCK into idle state */
	STCK = 0;
	STMS = 1;

	/* Make the "fast" outputs TDI and TCK push-pull */
	STDI_MODE |= 1 << STDI_BIT;
	STCK_MODE |= 1 << STCK_BIT;
	STMS_MODE |= 1 << STMS_BIT;

	/* Reset the TAP */
	nSTRST = 0;
	nSTRST = 1;
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

	/* Reset the TAP */
	nSTRST = 0;
	nSTRST = 1;
}
