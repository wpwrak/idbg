/*
 * This code follows the register read/write sequences from the examples in
 * SiLabs/MCU/Examples/C8051F326_7/USB_Interrupt/Firmware/F326_USB_Main.c and 
 * SiLabs/MCU/Examples/C8051F326_7/USB_Interrupt/Firmware/F326_USB_ISR.c
 */

#include <stdint.h>

#include "uart.h"
#include "usb.h"


#ifndef NULL
#define NULL 0
#endif


static void delay(void)
{
	int x;

	for (x = 0; x < 500; x)
		x++;
}


static uint8_t buf[10];


static void cb(void)
{
	printk("\"%s\"\n", buf);
}


static bit my_setup(struct setup_request *setup)
{
	printk("%x,%x;%x\n", setup->bmRequestType, setup->bRequest,
	  setup->wLength);
	if (setup->bRequest)
		return 0;
	if (setup->bmRequestType == 0x40) {
		putchar('<');
		usb_recv(&ep0, buf, setup->wLength, cb, NULL);
	}
	if (setup->bmRequestType == 0xc0) {
		putchar('>');
		usb_send(&ep0, "HI", 2, NULL, NULL);
	}
	return 1;
}


void dfu_init(void)
{
	user_setup = my_setup;
}
