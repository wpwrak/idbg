/*
 * common/usb-regs.h - C8051F326 USB register definitions
 *
 * Written 2008 by Werner Almesberger
 * Copyright 2008 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef USB_REGS_H
#define USB_REGS_H

/* Indirect USB registers */

#define	FADDR		0x00	/* Function Address */

#define	POWER		0x01	/* Power Management */
#define	SUSEN		0x01	/* Suspend Detection Enable */
#define	SUSMD		0x02	/* Suspend Mode */
#define	RESUME		0x04	/* Force Resume */
#define	USBRST		0x08	/* Reset Detect */
#define	USBINH		0x10	/* USB0 Inhibit */
#define	ISOUD		0x80	/* ISO Update */

#define	IN1INT		0x02	/* EP0 and EP1 IN Interrupt Flags */
#define	EP0		0x01	/* EP0 Interrupt-pending Flag */
#define	IN1		0x02	/* IN EP1 Interrupt-pending Flag */

#define	OUT1INT		0x04	/* EP1 OUT Interrupt Flag */

#define	CMINT		0x06	/* Common USB Interrupt Flags */
#define	SUSINT		0x01	/* Suspend Interrupt-Pending Flag */
#define	RSUINT		0x02	/* Resume Interrupt-Pending Flag */
#define	RSTINT		0x04	/* Reset Interrupt-Pending Flag */
#define	SOF		0x08	/* Start of Frame Interrupt */

#define	IN1IE		0x07	/* EP0 and EP1 IN Interrupt Enables */

#define	OUT1IE		0x09	/* EP1 out Interrupt Enable */

#define	CMIE		0x0b	/* Common USB Interrupt Enable */

#define	FRAMEL		0x0c	/* Frame Number Low Byte */

#define	FRAMEH		0x0d	/* Frame Number Low Byte */

#define	INDEX		0x0e	/* USB0 EP Index */

#define	CLKREC		0x0f	/* Clock Recovery Control */
#define	CRLOW		0x20	/* Low Speed Clock Recovery Mode */
#define	CRSSEN		0x40	/* Clock Recovery Single Step */
#define	CRE		0x80	/* Clock Recovery Enable */

#define	E0CSR		0x11	/* EP0 Control/Status */
#define	OPRDY_0		0x01	/* OUT Packet Ready */
#define	INPRDY_0	0x02	/* IN Packet Ready */
#define	STSTL_0		0x04	/* Sent Stall */
#define	DATAEND		0x08	/* Data End */
#define	SUEND		0x10	/* Setup End */
#define	SDSTL_0		0x20	/* Send Stall */
#define	SOPRDY		0x40	/* Serviced OPRDY */
#define	SSUEND		0x80	/* Serviced Setup End */

#define	EINCSRL		0x11	/* EP IN Control/Status Low Byte */
#define	INPRDY_IN	0x01	/* IN Packet Ready */
#define	FIFONE		0x02	/* FIFO Not Empty */
#define	UNDRUN		0x04	/* Data Underrun */
#define	FLUSH_IN	0x08	/* FIFO Flush */
#define	SDSTL_IN	0x10	/* Send Stall */
#define	STSTL_IN	0x20	/* Sent Stall */
#define	CLRDT_IN	0x40	/* Clear Data Toggle */

#define	EINCSRH		0x12	/* EP OUT Control/Status High Byte */

#define	EOUTCSRL	0x14	/* EP OUT Control/Status Low Byte */
#define	OPRDY_OUT	0x01	/* OUT  Packet Ready */
#define	FIFOFUL		0x02	/* OUT FIFO Full */
#define	OVRUN		0x04	/* Data Overrun */
#define	DATERR		0x08	/* Data Error */
#define	FLUSH_OUT	0x10	/* FIFO Flush */
#define	SDSTL_OUT	0x20	/* Send Stall */
#define	STSTL_OUT	0x40	/* Sent Stall */
#define	CLRDT_OUT	0x80	/* Clear Data Toggle */

#define	EOUTCSRH	0x15	/* EP OUT Control/Status High Byte */

#define	E0CNT		0x16	/* Number of Received Bytes in EP0 FIFO */

#define	EOUTCNTL	0x16	/* EP OUT Packet Count Low Byte */

#define	EOUTCNTH	0x17	/* EP OUT Packet Count High Byte */

#define	FIFO0		0x20	/* EP0 FIFO */

#define	FIFO1		0x21	/* EP1 FIFO */

#endif /* !USB_REGS_H */
