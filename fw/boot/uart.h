#ifndef UART_H
#define UART_H

#define print	uart_send_noint	/* sort of printk ... */


void uart_send_noint(const char *s);
void uart_init(void);

#endif /* UART_H */
