#ifndef UART_H
#define UART_H

#include <stdint.h>

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
void printk(const char *fmt, ...);
#else
#define printk(...)
#endif

void putchar(char c);
void uart_init(uint8_t brg_mhz);

#endif /* UART_H */
