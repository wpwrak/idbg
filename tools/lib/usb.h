/*
 * lib/usb.h - Common USB code for IDBG tools
 *
 * Written 2008, 2009 by Werner Almesberger
 * Copyright 2008, 2009 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef IDBG_USB_H
#define IDBG_USB_H

#include <usb.h>


usb_dev_handle *open_usb(void);
void parse_usb_id(const char *id);

#endif /* !IDBG_USB_H */
