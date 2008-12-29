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

#include "usb.h"


void i2c_start(uint8_t slave, const uint8_t *wdata, uint8_t wlen,
    uint8_t *rdata, uint8_t rlen);
__bit i2c_fetch(struct ep_descr *ep, uint8_t len);
void i2c_poll(void);

#endif /* !I2C_H */
