#include "uart.h"
#include "i2c.h"


bit i2c_write(uint8_t device, uint8_t addr, const uint8_t *buf, uint8_t len)
{
	printk("i2c_write\n");
	return 0;
}


bit i2c_read(uint8_t device, uint8_t addr, uint8_t *buf, uint8_t len)
{
	printk("i2c_read\n");
	return 0;
}
