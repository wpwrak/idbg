/*
 * lib/identify.h - Identify an IDBG board
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef IDBG_IDENTIFY_H
#define IDBG_IDENTIFY_H

#include <stdint.h>
#include <stdio.h>

#include <usb.h>


int idbg_get_protocol(usb_dev_handle *dev, uint8_t *major, uint8_t *minor);
int idbg_get_hw_type(usb_dev_handle *dev);
int idbg_print_id(FILE *file, usb_dev_handle *dev);

/*
 * Note: idbg_print_id also decodes errors and print error messages to stderr.
 */

#endif /* !IDBG_IDENTIFY_H */
