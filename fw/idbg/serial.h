/*
 * idbg/serial.h - Serial console
 *
 * Written 2008, 2009 by Werner Almesberger
 * Copyright 2008, 2009 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef SERIAL_H
#define SERIAL_H

#include "usb.h"


/*
 * @@@ This is slighly backwards. We shouldn't follow the buffer size the USB
 * side uses but the USB side (descr.c) should use the buffer size we choose
 * here.
 */

#define SERIAL_BUF_SIZE	EP1_SIZE


void serial_poll(void);
void serial_init(void);

#endif /* !SERIAL_H */
