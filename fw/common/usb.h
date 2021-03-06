/*
 * common/usb.h - USB hardware setup and standard device requests
 *
 * Written 2008, 2009 by Werner Almesberger
 * Copyright 2008, 2009 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef USB_H
#define USB_H


#include <stdint.h>

#include "config.h"


/*
 * Descriptor types
 *
 * Reuse libusb naming scheme (/usr/include/usb.h)
 */

#define	USB_DT_DEVICE		1
#define	USB_DT_CONFIG		2
#define	USB_DT_STRING		3
#define	USB_DT_INTERFACE	4
#define	USB_DT_ENDPOINT		5

/*
 * Device classes
 *
 * Reuse libusb naming scheme (/usr/include/usb.h)
 */

#define USB_CLASS_PER_INTERFACE	0xfe
#define USB_CLASS_VENDOR_SPEC	0xff

/*
 * Configuration attributes
 */

#define	USB_ATTR_BUS_POWERED	0x80
#define	USB_ATTR_SELF_POWERED	0x40
#define	USB_ATTR_REMOTE_WAKEUP	0x20

/*
 * Setup request types
 */

#define	TO_DEVICE(req)		(0x00 | (req) << 8)
#define	FROM_DEVICE(req)	(0x80 | (req) << 8)
#define	TO_INTERFACE(req)	(0x01 | (req) << 8)
#define	FROM_INTERFACE(req)	(0x81 | (req) << 8)
#define	TO_ENDPOINT(req)	(0x02 | (req) << 8)
#define	FROM_ENDPOINT(req)	(0x82 | (req) << 8)

/*
 * Setup requests
 */

#define	GET_STATUS		0x00
#define	CLEAR_FEATURE		0x01
#define	SET_FEATURE		0x03
#define	SET_ADDRESS		0x05
#define	GET_DESCRIPTOR		0x06
#define	SET_DESCRIPTOR		0x07
#define	GET_CONFIGURATION	0x08
#define	SET_CONFIGURATION	0x09
#define	GET_INTERFACE		0x0a
#define	SET_INTERFACE		0x0b
#define	SYNCH_FRAME		0x0c


/*
 * Odd. sdcc seems to think "x" assumes the size of the destination, i.e.,
 * uint8_t. Hence the cast.
 */

#define LE(x) ((uint16_t) (x) & 0xff), ((uint16_t) (x) >> 8)

#define LO(x) (((uint8_t *) &(x))[0])
#define HI(x) (((uint8_t *) &(x))[1])


#ifdef LOW_SPEED
#define	EP0_SIZE	8
#else
#define	EP0_SIZE	64
#endif

#define	EP1_SIZE	64	/* simplify */


enum ep_state {
	EP_IDLE,
	EP_RX,
	EP_TX,
	EP_STALL,
};

struct ep_descr {
	enum ep_state state;
	uint8_t *buf;
	uint8_t *end;
	void (*callback)(void *user) __reentrant;
	void *user;
};

struct setup_request {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
};


extern const uint8_t device_descriptor[];
extern const uint8_t config_descriptor[];
extern __xdata struct ep_descr ep0;
#ifdef CONFIG_EP1
extern __xdata struct ep_descr ep1in, ep1out;
#endif

extern __bit (*user_setup)(struct setup_request *setup) __reentrant;
extern __bit (*user_get_descriptor)(uint8_t type, uint8_t index,
    const uint8_t * const *reply, uint8_t *size) __reentrant;
extern void (*user_reset)(void) __reentrant;


#define	usb_left(ep) ((ep)->end-(ep)->buf)
#define usb_send(ep, buf, size, callback, user) \
	usb_io(ep, EP_TX, buf, size, callback, user)
#define usb_recv(ep, buf, size, callback, user) \
	usb_io(ep, EP_RX, buf, size, callback, user)

void usb_io(struct ep_descr *ep, enum ep_state state, uint8_t *buf,
    uint8_t size, void (*callback)(void *user), void *user);


void usb_init(void);
void usb_poll(void);

#endif /* !USB_H */
