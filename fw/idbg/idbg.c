#include "uart.h"
#include "usb.h"
#include "version.h"


void main(void)
{
	/* @@@ since boot and payload may run at different USB speeds, we
	   should also redo the clock init */
	uart_init(24);

	printk("Hello, payload\n");
	printk("%s #%u\n", build_date, build_number);

	usb_init();
	while (1);
}
