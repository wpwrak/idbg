/*
 * gpio.h - Really primitive S3C244x GPIO access. Only works for ports B-H.
 */


/*
 * Ports are numbered 0 = A, 1 = B, ...
 */


#ifndef GPIO_H
#define GPIO_H


#include <stdint.h>


volatile uint32_t *mem;


#define port_dat(port) mem[port*4+1]
#define port_con(port) mem[port*4]


static inline void gpio_high(unsigned port, unsigned bit)
{
    port_dat(port) |= 1 << bit;
}


static inline void gpio_low(unsigned port, unsigned bit)
{
    port_dat(port) &= ~(1 << bit);
}


static inline void gpio_set(unsigned port, unsigned bit, int value)
{
    if (value)
	gpio_high(port, bit);
    else
	gpio_low(port, bit);
}


static inline int gpio_get(unsigned port, unsigned bit)
{
    return (port_dat(port) >> bit) & 1;
}


static inline void gpio_output(unsigned port, unsigned bit)
{
    port_con(port) = (port_con(port) & ~(3 << bit*2)) | (1 << bit*2);
}


static inline void gpio_input(unsigned port, unsigned bit)
{
    port_con(port) &= ~(3 << bit*2);
}


void gpio_init(void);


#endif /* !GPIO_H */
