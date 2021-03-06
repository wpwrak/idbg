/*
 * common/io.h - I/O pin assignment
 *
 * Written 2008, 2010 by Werner Almesberger
 * Copyright 2008, 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef IO_H
#define	IO_H

/* Serial console */

#define	CONSOLE_RXD	P0_4	/* debug -> Neo */
#define	CONSOLE_TXD	P0_5	/* Neo -> debug */


#if defined(GTA)

/* I2C */

#define	I2C_SDA		P0_2
#define	I2C_SCL		P0_0
#define	I2C_SDA_PULL	P0_1

/* JTAG */

#define	nSTRST		P0_6
#define	STCK		P0_7
#define	nRESET		P2_0
#define	STDO		P2_1
#define	STMS		P2_4
#define	STDI		P2_5

/* Special functions */

#define	nGSM_EN		P2_3	/* GTA01 only */
#define	nNOR_WP		P2_2	/* GTA02 only */
#define	FUTURE		P0_3

#elif defined(BEN_V1)

#define	BOOT_SEL1	P0_3
#define	RESETP_N	P3_0

#elif defined(BEN_V2)

#define	BOOT_SEL1	P3_0
#define	RESETP_N	P0_1

#else

#error	"Must define target (GTA, BEN_V1, or BEN_V2)"

#endif

#endif /* !IO_H */
