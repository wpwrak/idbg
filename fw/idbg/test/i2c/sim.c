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

#include "../../i2c.c"


static void polln(int n)
{
	int i;

	for (i = 0; i != n; i++)
		i2c_poll();
}


int main(void)
{
	int i;

	I2C_SDA = I2C_SCL = 1;
	i2c_start(0x17, "A", 1, NULL, 0);
	polln(20);
	I2C_SDA = 0;
	i2c_poll();
	polln(20);
	I2C_SDA = 0;
	i2c_poll();
	polln(20);
	fprintf(stderr, "state %d\n", state);
	i2c_fetch(NULL, 10);
	return 0;
}
