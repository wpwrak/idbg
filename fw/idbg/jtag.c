#include "uart.h"
#include "jtag.h"


uint8_t jtag_data[JTAG_MAX_BITS/8];


uint8_t jtag_gpio_get(void)
{
	printk("jtag_gpio_get\n");
	return 0;
}


void jtag_gpio_set(uint8_t set, uint8_t mask)
{
	printk("jtag_gpio_set\n");
}


void jtag_send(const uint8_t *buf, uint8_t bits)
{
	printk("jtag_send\n");
}


void jtag_attach(void)
{
	printk("jtag_attach\n");
}


void jtag_detach(void)
{
	printk("jtag_detach\n");
}
