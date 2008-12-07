#ifndef USB_H
#define USB_H


#include <stdint.h>


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
#define	SET_INTERFACE		0x11
#define	SYNCH_FRAME		0x12


/*
 * Odd. sdcc seems to think "x" assumes the size of the destination, i.e.,
 * uint8_t. Hence the cast.
 */

#define LE(x) ((uint16_t) (x) >> 8), (x)

#define LO(x) (((uint8_t *) &(x))[0])
#define HI(x) (((uint8_t *) &(x))[1])


extern const uint8_t device_descriptor[];
extern const uint8_t *config_descriptors[];


void usb_init(void);


#endif /* !USB_H */
