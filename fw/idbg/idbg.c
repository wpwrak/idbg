#include "uart.h"
#include "usb.h"
#include "version.h"


void main(void)
{
	usb_init();
	uart_init(24);

	printk("Hello, payload\n");
	printk("%s #%u\n", build_date, build_number);
	while (1);
}
