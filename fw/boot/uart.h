#ifndef UART_H
#define UART_H

#define debug printk

void putchar(char c);
void printk(const char *fmt, ...);
void uart_init(void);

#endif /* UART_H */
