#ifndef UART_H
#define UART_H

#if 0
#define debug printk
#else
#define debug(...)
#endif

void putchar(char c);
void printk(const char *fmt, ...);
void uart_init(void);

#endif /* UART_H */
