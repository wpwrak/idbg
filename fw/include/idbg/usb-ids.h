/*
 * include/idbg/usb-ids.h - USB vendor and product IDs
 *
 * Written 2009, 2010 by Werner Almesberger
 * Copyright 2009, 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef USB_IDS_H
#define USB_IDS_H

#define USB_VENDOR_OPENMOKO	0x1d50	/* Openmoko Inc. */
#define USB_VENDOR_QI_HW	0x20b7	/* Qi Hardware */
#define USB_PRODUCT_IDBG_DFU	0x1db5	/* IDBG in DFU mode */
#define USB_PRODUCT_IDBG	0x1db6	/* IDBG in normal mode */


#if defined(GTA)

#define USB_VENDOR		USB_VENDOR_OPENMOKO

#elif defined(BEN_V1) || defined(BEN_V2)

/*
 * Note: the two original V1 boards use USB_VENDOR_OPENMOKO if not updated.
 */

#define USB_VENDOR		USB_VENDOR_QI_HW

#endif

#endif /* !USB_IDS_H */
