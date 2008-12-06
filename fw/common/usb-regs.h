#ifndef USB_REGS_H
#define USB_REGS_H

/* Indirect USB registers */

#define	FADDR		0x00	/* Function Address */

#define	POWER		0x01	/* Power Management */
#define	SUSEN		0x01	/* Suspend Detection Enable */
#define	SUSMD		0x02	/* Suspend Mode */
#define	RESUME		0x04	/* Force Resume *
#define	USBRST		0x08	/* Reset Detect */
#define	USBINH		0x10	/* USB0 Inhibit */
#define	ISOUD		0x80	/* ISO Update */

#define	IN1INT		0x02	/* EP0 and EP1 IN Interrupt Flags */

#define	OUT1INT		0x04	/* EP1 OUT Interrupt Flag */

#define	CMINT		0x06	/* Common USB Interrupt Flags */

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

#define	E0SCR		0x11	/* EP0 Control/Status */

#define	EINCSRL		0x11	/* EP IN Control/Status Low Byte */

#define	EINCSRH		0x11	/* EP OUT Control/Status High Byte */

#define	EOUTCSRL	0x14	/* EP OUT Control/Status Low Byte */

#define	EOUTCSRH	0x14	/* EP OUT Control/Status High Byte */

#define	E0CNT		0x16	/* Number of Received Bytes in EP0 FIFO */

#define	EOUTCNTL	0x16	/* EP OUT Packet Count Low Byte */

#define	EOUTCNTH	0x17	/* EP OUT Packet Count High Byte */

#define	FIFO0		0x20	/* EP0 FIFO */

#define	FIFO1		0x21	/* EP1 FIFO */

#endif /* USB_REGS_H */
