/*
 * idbg/ep0idbg.h - EP0 extension protocol
 *
 * Written 2008, 2009 by Werner Almesberger
 * Copyright 2008, 2009 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef EP0IDBG_H
#define EP0IDBG_H

/*
 * Direction	bRequest		wValue		wIndex	wLength
 *
 * ->host	IDBG_ID			0		0	2
 * host->	IDBG_RESET		0		0	0
 *
 * host->	IDBG_JTAG_ATTACH	0		0	0
 * host->	IDBG_JTAG_DETACH	0		0	0
 * host->	IDBG_JTAG_GPIO_SET	value		mask	0
 * ->host	IDBG_JTAG_GPIO_GET	0		0	1
 * host->	IDBG_JTAG_SCAN		#bits		last	#bytes
 * ->host	IDBG_JTAG_FETCH		#bits		0	#bytes
 * host->	IDBG_JTAG_MOVE		#bits		0	#bytes
 *
 * host->	IDBG_I2C_WRITE		address		device	#bytes
 * ->host	IDBG_I2C_READ		address		device	#bytes
 * ->host	IDBG_I2C_FETCH		0		0	#bytes
 *
 * host->	IDBG_GPIO_UPDATE	0		mask	0
 * host->	IDBG_GPIO_DATA_SET	value		mask	0
 * ->host	IDBG_GPIO_DATA_GET	0		0	2
 * host->	IDBG_GPIO_MODE_SET	value		mask	0
 * ->host	IDBG_GPIO_MODE_GET	0		0	2
 */


#define EP0IDBG_MAJOR	0
#define EP0IDBG_MINOR	0


/*
 * bmRequestType:
 *
 * D7 D6..5 D4...0
 * |  |     |
 * direction (0 = host->dev)
 *    type (2 = vendor)
 *          recipient (0 = device)
 */


#define	IDBG_TO_DEV(req)	(0x40 | (req) << 8)
#define	IDBG_FROM_DEV(req)	(0xc0 | (req) << 8)


enum idbg_requests {
	IDBG_ID			= 0x00,
	IDBG_RESET,
	IDBG_JTAG_ATTACH	= 0x10,
	IDBG_JTAG_DETACH,
	IDBG_JTAG_GPIO_SET,
	IDBG_JTAG_GPIO_GET,
	IDBG_JTAG_SCAN,
	IDBG_JTAG_FETCH,
	IDBG_JTAG_MOVE,
	IDBG_I2C_WRITE		= 0x20,
	IDBG_I2C_READ,
	IDBG_I2C_FETCH,
	IDBG_GPIO_UPDATE	= 0x30,
	IDBG_GPIO_DATA_SET,
	IDBG_GPIO_DATA_GET,
	IDBG_GPIO_MODE_SET,
	IDBG_GPIO_MODE_GET,
};


void ep0idbg_init(void);

#endif /* !EP0IDBG_H */
