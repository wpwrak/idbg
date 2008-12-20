#ifndef UART_H
#define UART_H

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
void uart_init(void);

#endif /* UART_H */
