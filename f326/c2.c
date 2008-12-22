#include <stdint.h>
#include <unistd.h>

#include "gpio.h"
#include "c2.h"


/*
 * SPI GPIOs are the same on 2410 and 2442, so this should work on GTA01 and
 * on GTA02.
 */

#define C2CK	4, 11	/* E13 = SPI_MISO0 */
#define C2D	4, 13	/* E12 = SPI_CLK0 */


/* ----- Bit-level operations ---------------------------------------------- */


static void c2_pulse(void)
{
    gpio_low(C2CK);
    gpio_high(C2CK);
}


static void c2_send(uint32_t value, int bits)
{
    int i;

    for (i = 0; i != bits; i++) {
	gpio_set(C2D, (value >> i) & 1);
	c2_pulse();
    }
}


static uint32_t c2_recv(int bits)
{
    uint32_t v = 0;
    int i;

    for (i = 0; i != bits; i++) {
	v |= gpio_get(C2D) << i;
	c2_pulse();
    }
    return v;
}


/* ----- C2 Register read/write -------------------------------------------- */


void c2_addr_write(uint8_t addr)
{
    c2_pulse();
    gpio_output(C2D);
    c2_send(C2_ADDR_WRITE, 2);
    c2_send(addr, 8);
    gpio_input(C2D);
    c2_pulse();
}


uint8_t c2_addr_read(void)
{
    c2_pulse();
    gpio_output(C2D);
    c2_send(C2_ADDR_READ, 2);
    gpio_input(C2D);
    c2_pulse();
    return c2_recv(8);
}


void c2_data_write(uint32_t data, int bytes)
{
    c2_pulse();
    gpio_output(C2D);
    c2_send(C2_DATA_WRITE, 2);
    c2_send(bytes-1, 2);
    c2_send(data, 8*bytes);
    gpio_input(C2D);
    c2_pulse();
    while (!c2_recv(1));
}


uint32_t c2_data_read(int bytes)
{
    c2_pulse();
    gpio_output(C2D);
    c2_send(C2_DATA_READ, 2);
    c2_send(bytes-1, 2);
    gpio_input(C2D);
    c2_pulse();
    while (!c2_recv(1));
    return c2_recv(8*bytes);
}


/* ----- C2 initialization ------------------------------------------------- */


void c2_init(void)
{
    gpio_init();
    gpio_input(C2D);
    gpio_output(C2CK);
    gpio_low(C2CK);
    usleep(20);
    gpio_high(C2CK);
    usleep(2);
}


void c2_reset(void)
{
    gpio_input(C2D);
    gpio_low(C2CK);
    usleep(20);
//    gpio_input(C2CK);
    gpio_output(C2CK);
    gpio_high(C2CK);
}
