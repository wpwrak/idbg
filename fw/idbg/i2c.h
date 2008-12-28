/*
 * idbg/i2c.h - I2C protocol engine
 *
 * Written 2008 by Werner Almesberger
 * Copyright 2008 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef I2C_H
#define I2C_H

#include <stdint.h>


/* @@@ we need a bit better signaling for asynchronous operation */

__bit i2c_write(uint8_t device, uint8_t addr, const uint8_t *buf, uint8_t len);
__bit i2c_read(uint8_t device, uint8_t addr, uint8_t *buf, uint8_t len);

#endif /* !I2C_H */
