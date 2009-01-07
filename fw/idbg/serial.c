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


/*
 * USB Request Blocks
 */

#define	RX_URBS	10	/* that's probably too many, but XRAM is cheap :-) */
#define	TX_URBS	2


static __xdata struct urb {
	uint8_t buf[SERIAL_BUF_SIZE];
	uint8_t len;
	__xdata struct urb *next;
} rx_urbs[RX_URBS], tx_urbs[TX_URBS];

static __xdata struct queue {
	__xdata struct urb *volatile head;
	__xdata struct urb *volatile tail; /* undefined if head == NULL */
} rx_uart_q, rx_wait_q, tx_uart_q, tx_usb_q;

static __bit txing = 0; /* UART is txing */
static uint8_t tx_pos = 0;

static volatile __xdata struct urb *rxing = NULL;
static volatile uint8_t rx_pos = 0;


/* ----- Queue handling ---------------------------------------------------- */


#pragma nooverlay

static void enqueue(__xdata struct queue *q, __xdata struct urb *urb)
{
	if (q->head) {
		q->tail->next = urb;
	} else {
		q->head = urb;
	}
	q->tail = urb;
	urb->next = NULL;
}


#pragma nooverlay

static __xdata struct urb *dequeue(__xdata struct queue *q)
{
	__xdata struct urb *urb = q->head;
	
	if (urb)
		q->head = urb->next;
	return urb;
}


static void init_queue(struct queue *q, __xdata struct urb *urbs,
    uint8_t n_urbs)
{
	uint8_t i;

	q->head = urbs;
	q->tail = urbs+n_urbs-1;
	for (i = 0; i != n_urbs; i++)
		urbs[i].next = i == n_urbs-1 ? NULL : urbs+i+1;
}


/* ----- Neo -> Host ------------------------------------------------------- */


static void got_tx(void *user)
{
	__xdata struct urb *urb = user;

	urb->len = SERIAL_BUF_SIZE-usb_left(&ep1out);
	enqueue(&tx_uart_q, dequeue(&tx_usb_q));
	urb = tx_usb_q.head;
	if (urb)
		usb_recv(&ep1out, urb->buf, SERIAL_BUF_SIZE, got_tx, urb);
}


static void do_tx(void)
{
	__xdata struct urb *urb;

	urb = tx_uart_q.head;
	if (!urb) {
		txing = 0;
		return;
	}
	txing = 1;
	SBUF0 = urb->buf[tx_pos];
	tx_pos++;
	if (tx_pos != urb->len)
		return;
	tx_pos = 0;
	enqueue(&tx_usb_q, dequeue(&tx_uart_q));
	if (tx_usb_q.head == urb)
		usb_recv(&ep1out, urb->buf, SERIAL_BUF_SIZE, got_tx, urb);
}


/* ----- Host -> Neo ------------------------------------------------------- */


#pragma nooverlay

static void rx_submit(void)
{
	__xdata struct urb *urb;

	urb = dequeue(&rx_uart_q);
	if (!urb)
		return;
	urb->len = rx_pos;
	enqueue(&rx_wait_q, urb);
	rx_pos = 0;
}


static void rx_done(void *user) __critical
{
	user; /* silence sdcc */

	enqueue(&rx_uart_q, rxing);
	rxing = NULL;
	/*
	 * Check if we're idle but have a partially filled URB in the UART
	 * queue.
	 */
	if (rx_pos && !rx_wait_q.head)
		rx_submit();
}


static void do_rx(void)
{
	if (rxing)
		return;
	__critical {
		rxing = dequeue(&rx_wait_q);
		if (rxing)
			usb_send(&ep1in, rxing->buf, rxing->len, rx_done, NULL);
	}
}


#pragma nooverlay

static void rx_enqueue(char ch) __critical
{
	__xdata struct urb *urb;

	urb = rx_uart_q.head;
	if (!urb)
		return;
	urb->buf[rx_pos] = ch;
	rx_pos++;
	/*
	 * Put the current URB into the USB wait queue if:
	 * - we're idle, or
	 * - it is full
	 */
	if ((!rxing && !rx_wait_q.head) || rx_pos == SERIAL_BUF_SIZE)
		rx_submit();
}


void uart_isr(void) __interrupt(4)
{
	if (!RI0)
		return;
	rx_enqueue(SBUF0);
	RI0 = 0;
}


#ifdef CONFIG_USB_PUTCHAR

void putchar(char ch) __critical
{
	rx_enqueue(ch);
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
	init_queue(&tx_usb_q, tx_urbs, TX_URBS);
	init_queue(&tx_uart_q, NULL, 0);
	init_queue(&rx_uart_q, rx_urbs, RX_URBS);
	init_queue(&rx_wait_q, NULL, 0);
	usb_recv(&ep1out, tx_urbs->buf, SERIAL_BUF_SIZE, got_tx, tx_urbs);
	REN0 = 1; /* enable receiver */
	ES0 = 1; /* enable interrupt */
}
