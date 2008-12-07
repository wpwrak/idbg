#ifndef REGS_H
#define REGS_H

#include <mcs51/C8051F326.h>
#include "usb-regs.h"


/* OSCICN */
#define	IFCN0	0x01	/* Internal Oscillator Frequency Control */
#define	IFCN1	0x02	/* 00: /8, 01: /4, 10: /2, 11: /1 */

/* CLKMUL */
#define	MULSEL	0x01	/* Clock Multiplier Input Select */
#define	MULRDY	0x20	/* Clock Multiplier Ready */
#define	MULINIT	0x40	/* Clock Multiplier Initialize */
#define	MULEN	0x80	/* Clock Multiplier Enable */

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
