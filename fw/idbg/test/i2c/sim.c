#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "io.h"


static void __usb_send(uint8_t *buf, int len)
{
	int i;

	fprintf(stderr, "usb_send(%d):", len);
	for (i = 0; i != len; i++)
		fprintf(stderr, " %02X", buf[i]);
	fputc('\n', stderr);
}


static void delay(void)
{
	fprintf(stderr, "SDA %d SCL %d\n", I2C_SDA, I2C_SCL);
}


#define SIMULATION
#define printk(ARGS...) fprintf(stderr, ARGS)

#include "../../i2c.c"


uint8_t buf[20];

static void polln(int n)
{
	int i;

	for (i = 0; i != n; i++)
		i2c_poll();
}


static void i2c_write(uint8_t slave, uint8_t addr, const uint8_t *data,
   int len)
{
	buf[0] = slave << 1;
	buf[1] = addr;
	memcpy(buf+2, data, len);
	i2c_start(0, buf, len+2, NULL, 0);
}


static void i2c_read(uint8_t slave, uint8_t addr, uint8_t *data, int len)
{
	buf[0] = slave << 1;
	buf[1] = addr;
	buf[2] = slave << 1 | 1;
	i2c_start(2, buf, 3, buf, len);
}


int main(void)
{
	int i;
	uint8_t tmp;

	I2C_SDA = I2C_SCL = 1;
	delay();

#if 1
	i2c_write(0x73, 0, "A", 1);
#else
	i2c_read(0x73, 0, &tmp, 1);
#endif

	polln(200);
	delay();
	delay();
	delay();
	delay();
	fprintf(stderr, "state %d\n", state);
	i2c_fetch(NULL, 10);
	i2c_fetch(NULL, 10);

	return 0;
}
