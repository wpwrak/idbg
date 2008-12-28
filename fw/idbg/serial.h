/*
 * idbg/serial.h - Serial console
 *
 * Written 2008 by Werner Almesberger
 * Copyright 2008 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef SERIAL_H
#define SERIAL_H

#include "usb.h"


#define TX_BUF_SIZE	EP1_SIZE
#define RX_BUF_SIZE	EP1_SIZE


void serial_poll(void);
void serial_init(void);

#endif /* !SERIAL_H */
