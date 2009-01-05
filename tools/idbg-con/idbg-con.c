/*
 * idbg-con/idbg-con.c - IDBG console
 *
 * Written 2008, 2009 by Werner Almesberger
 * Copyright 2008, 2009 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * Polling is a bit crude but it helps us to avoid the complexity of
 * asynchronous libusb. Besides, one should rather use the in-kernel USB serial
 * driver anyway.
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <usb.h>


static struct termios console;


static usb_dev_handle *open_usb(void)
{
	const struct usb_bus *bus;
	struct usb_device *dev;

	usb_init();
	usb_find_busses();
	usb_find_devices();

	for (bus = usb_get_busses(); bus; bus = bus->next)
		for (dev = bus->devices; dev; dev = dev->next) {
			if (dev->descriptor.idVendor != 0x1234)
				continue;
			if (dev->descriptor.idProduct != 0x0002)
				continue;
			return usb_open(dev);
		}
	return NULL;
}


static void make_raw(int fd, struct termios *old)
{
	struct termios t;
	unsigned long flags;

	if (tcgetattr(fd, &t) < 0) {
		perror("tcgetattr");
		exit(1);
	}
	*old = t;
	cfmakeraw(&t);
	if (tcsetattr(fd, TCSANOW, &t) < 0) {
		perror("tcsetattr");
		exit(1);
	}
	flags = fcntl(fd, F_GETFL);
	if (flags < 0) {
		perror("fcntl F_GETFL");
		exit(1);
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
		perror("fcntl F_GETFL");
		exit(1);
	}
}


static void cleanup(void)
{
	if (tcsetattr(0, TCSANOW, &console) < 0)
		perror("tcsetattr");
	write(1, "\n", 1);
}


static int do_poll(usb_dev_handle *dev)
{
	static int escape = 0;
	char buf[100];
	int res, i;
	ssize_t got;

	res = usb_bulk_read(dev, 0x81, buf, sizeof(buf), 10);
	if (res < 0 && res != -ETIMEDOUT)
		return 0;
	if (res > 0)
		write(1, buf, res);
	got = read(0, buf, sizeof(buf));
	if (got < 0) {
		if (errno == EAGAIN)
			return 1;
		perror("read");
		exit(1);
	}
	for (i = 0; i != got; i++)
		switch (buf[i]) {
		case '~':
			escape = 1;
			break;
		case '.':
			if (escape)
				exit(0);
			/* fall through */
		default:
			escape = 0;
		}
	res = usb_bulk_write(dev, 0x01, buf, got, 1000);
	if (res < 0)
		return 0;
	if (res != got) {
		fprintf(stderr, "short write\n");
		exit(1);
	}
	return 1;
}


int main(int argc, const char **argv)
{
	usb_dev_handle *dev;
	int reported = 0;
	int res;

	make_raw(0, &console);
	atexit(cleanup);
	while (1) {
		dev = open_usb();
		if (dev) {
			res = usb_claim_interface(dev, 0);
			if (res >= 0) {
				fprintf(stderr, "[ Connected ]\r\n");
				reported = 0;
				while (do_poll(dev));
			}
			usb_close(dev);
		}
		if (!reported)
			fprintf(stderr, "[ Waiting for device ]\r\n");
		reported = 1;
		usleep(100000);
	}
	return 0;
}
