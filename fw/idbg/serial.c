/*
 * idbg/serial.c - Serial console
 *
 * Written 2008, 2009 by Werner Almesberger
 * Copyright 2008, 2009 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>

#include "config.h"
#include "regs.h"
#include "uart.h"
#include "usb.h"
#include "serial.h"


#ifndef NULL
#define NULL 0
#endif


static __xdata struct urb {
	uint8_t buf[TX_BUF_SIZE];
	uint8_t *pos;
	uint8_t *end;
} urbs[2];

static struct urb *__xdata uart_q[2] = { NULL, NULL };
static struct urb *__xdata usb_q[2] = { urbs, urbs+1 };
static __bit txing = 0; /* UART is txing */

static __xdata uint8_t rx_buf[2][RX_BUF_SIZE];
static __bit rx_curr = 0, rx_busy = 0;
static volatile uint8_t rx_pos = 0;


static void got_tx(void *user)
{
	struct urb *urb = user;

	urb->pos = urb->buf;
	/*
	 * Weird. SDCC 2.7.0 and 2.8.0 don't seem to know that "array" is
	 * equivalent to &array[0] and get confused. @@@
	 */
	urb->end = &urb->buf[0]+TX_BUF_SIZE-usb_left(&ep1out);
	usb_q[0] = usb_q[1];
	usb_q[1] = NULL;
	if (uart_q[0]) {
		uart_q[1] = urb;
	} else {
		uart_q[0] = urb;
	}
	urb = usb_q[0];
	if (urb)
		usb_recv(&ep1out, urb->buf, TX_BUF_SIZE, got_tx, urb);
}


static void do_tx(void)
{
	struct urb *urb;

	urb = uart_q[0];
	if (!urb) {
		txing = 0;
		return;
	}
	txing = 1;
	SBUF0 = *urb->pos++;
	if (urb->pos != urb->end)
		return;
	uart_q[0] = uart_q[1];
	uart_q[1] = NULL;
	if (usb_q[0]) {
		usb_q[1] = urb;
	} else {
		usb_q[0] = urb;
		usb_recv(&ep1out, urb->buf, TX_BUF_SIZE, got_tx, urb);
	}
}


static void rx_done(void *user)
{
	user; /* silence sdcc */
	rx_busy = 0;
}


static void send_rx(void)
{
	usb_send(&ep1in, rx_buf[rx_curr], rx_pos, rx_done, NULL);
	rx_busy = 1;
	rx_curr = !rx_curr;
	rx_pos = 0;
}


static void do_rx(void)
{
	EA = 0;
	EA = 0;
	if (rx_pos && !rx_busy)
		send_rx();
	EA = 1;
}


void uart_isr(void) __interrupt(4)
{
	if (!RI0)
		return;
	RI0 = 0;
	if (rx_pos == RX_BUF_SIZE) {
		debug("do_rx overrun\n");
		return;
	}
	rx_buf[rx_curr][rx_pos] = SBUF0;
	rx_pos++;
}


#ifdef CONFIG_USB_PUTCHAR

void putchar(char ch)
{
	EA = 0;
	EA = 0;
	if (rx_pos != RX_BUF_SIZE) {
		rx_buf[rx_curr][rx_pos] = ch;
		rx_pos++;
	}
	EA = 1;
}

#endif /* CONFIG_USB_PUTCHAR */


void serial_poll(void)
{
	do_rx();
	if (TI0 || !txing) {
		TI0 = 0;
		do_tx();
	}
}


void serial_init(void)
{
	usb_recv(&ep1out, urbs->buf, TX_BUF_SIZE, got_tx, urbs);
	REN0 = 1; /* enable receiver */
	ES0 = 1; /* enable interrupt */
}
