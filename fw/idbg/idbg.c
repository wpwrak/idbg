#include "uart.h"
#include "usb.h"


void main(void)
{
	uart_init();
	usb_init();

	printk("Hello, payload\n");
}
