#ifndef IO_H
#define	IO_H

/* Serial console */

#define	CONSOLE_RXD	P0_4	/* debug -> Neo */
#define	CONSOLE_TXD	P0_5	/* Neo -> debug */

/* I2C */

#define	I2C_SDA		P0_2
#define	I2C_SCL		P0_3
#define	I2C_SDA_PULL	P0_1

/* JTAG */

#define	nSTRST		P0_6
#define	STCK		P0_7
#define	nRESET		P2_0
#define	STDO		P2_1
#define	STMS		P2_4
#define	STDI		P2_5

/* Special functions */

#define	GSM_EN		P2_3	/* GTA01 only */
#define	nNOR_WP		P2_2	/* GTA02 only */
#define	FUTURE		P0_0

#endif /* !IO_H */