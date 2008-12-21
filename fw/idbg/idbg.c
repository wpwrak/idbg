#include "regs.h"
#include "uart.h"
#include "usb.h"
#include "version.h"


static void usb_isr(void) __interrupt(8)
{
	putchar('&');
}


void main(void)
{
	/* @@@ since boot and payload may run at different USB speeds, we
	   should also redo the clock init */
	uart_init(24);

	printk("Hello, payload\n");
	printk("%s #%u\n", build_date, build_number);

EIE1 = 2;
EA = 1;

	usb_init();
	while (1)
		usb_poll();
}
