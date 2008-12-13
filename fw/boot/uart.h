#ifndef UART_H
#define UART_H

#if 0
#define debug printk
#else
#define debug(...)
#endif

#if 1
#define error printk
#else
#define error(...)
#endif

void putchar(char c);
void printk(const char *fmt, ...);
void uart_init(void);

#endif /* UART_H */
