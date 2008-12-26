/*
 * common/regs.h - C8051F326 register definitions
 *
 * Written 2008 by Werner Almesberger
 * Copyright 2008 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef REGS_H
#define REGS_H

#include <mcs51/C8051F326.h>
#include "usb-regs.h"


/* REG0CN */
#define	REGMOD	0x10	/* Voltage Regulator Mode Select */
#define	VBPOL	0x20	/* VBUS Interrupt Polarity Select */
#define	VBSTAT	0x40	/* VBUS Signal Status */
#define	REGDIS	0x80	/* Voltage Regulator Disable */

/* RSTSRC */
#define	PINRSF	0x01	/* HW Pin Reset Flag */
/*	PORSF	0x02	-- Power-On/VDD Monitor Reset Flag */
#define	MCFRSF	0x04	/* Missing Clock Detector Flag */
/*	SWRSF	0x10	-- Software Reset Force and Flag */
#define	FERROR	0x40	/* Flash Error Indicator */
#define	USBRSF	0x80	/* USB Reset Flag */

/* OSCICN */
#define	IFCN0	0x01	/* Internal Oscillator Frequency Control */
#define	IFCN1	0x02	/* 00: /8, 01: /4, 10: /2, 11: /1 */

/* CLKMUL */
#define	MULSEL	0x01	/* Clock Multiplier Input Select */
#define	MULRDY	0x20	/* Clock Multiplier Ready */
#define	MULINIT	0x40	/* Clock Multiplier Initialize */
#define	MULEN	0x80	/* Clock Multiplier Enable */

/* GPIOCN */
#define	SYSCLK	0x01	/* nSYSCLK Enable */
#define	INPUTEN	0x40	/* Global Digital Input Enable */
#define	WEAKPUD	0x80	/* Port I/O Weak Pullup Disable */

/* VDM0CN */
#define VDMEN	0x80	/* VDD Monitor Enable */
#define	VDDSTAT	0x40	/* VDD Status */

/* USB0XCN */
#define	Dn	0x01	/* D- Signal Status */
#define	Dp	0x02	/* D+ Signal Status */
#define	DFREC	0x04	/* Differential Receiver */
#define	PHYTST0	0x08	/* Physical Layer Test */
#define	PHYTST1	0x10	/* 00: normal, 01: "1", 10: "0", 11: SE0 */
#define	SPEED	0x20	/* USB0 Speed Select */
#define	PHYEN	0x40	/* Physical Layer Enable */
#define	PREN	0x80	/* Internal Pullup Resistor Enable */

/* USB0ADR */
#define	BUSY	0x80	/* USB0 Register Read Busy Flag */

/* SMOD0 */
#define	S0DL0	0x04	/* Data Length */
#define	S0DL1	0x08	/* 00: 5-bit, 01: 6-bit, 10: 7-bit, 11: 8-bit */

/* SBCON0 */
#define	SB0PS0	0x01	/* Baud Rate Prescaler Select */
#define	SB0PS1	0x02	/* 00: /12, 01: /4, 10: /48, 11: /1 */
#define	SB0RUN	0x40	/* Baud Rate Generator Enable */
#define	SB0CLK	0x80	/* Baud Rate Clock Source */

#endif /* REGS_H */
