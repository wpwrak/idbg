/*
 * boot/version.h - Automatically generated version string
 *
 * Written 2008 by Werner Almesberger
 * Copyright 2008 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef VERSION_H
#define VERSION_H

#include <stdint.h>

/*
 * Oddly, sdcc seems to insist on the "extern" to mean "declaration".
 */

extern const char *build_date;
extern const uint16_t build_number;

#endif /* !VERSION_H */
