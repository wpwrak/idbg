/*
 * common/uart.h - UART initialization and debug output
 *
 * Written 2008 by Werner Almesberger
 * Copyright 2008 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdio.h>

#include "config.h"


#ifdef CONFIG_DEBUG
#define debug printk
#define CONFIG_PRINTK
#else
#define debug(...)
#endif

#ifdef CONFIG_ERROR
#define error printk
#define CONFIG_PRINTK
#else
#define error(...)
#endif

#ifdef CONFIG_PRINTK
#define printk printf_fast
#else
#define printk(...)
#endif

void putchar(char c);
void uart_init(uint8_t brg_mhz);

#endif /* !UART_H */
