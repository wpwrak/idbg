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
 *
 * Note that the polling also has a race condition in the USB stack: it seems
 * that, if a packet arrives when we're just about to time out, the packet is
 * lost. We work around this issue by creating a thread that just blocks until
 * data has arrived.
 */


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <usb.h>
#include <linux/usbdevice_fs.h>

#include "idbg/usb-ids.h"
#include "../lib/usb.h"


static struct termios console;
static volatile int disconnected;


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


static int synchronous_bulk_read(usb_dev_handle *dev)
{
	int got;
	ssize_t wrote;
	char buf[100];
	struct usbdevfs_bulktransfer req = {
		.ep = 0x81,
		.len = sizeof(buf),
		.timeout = 0,
		.data = buf,
	};

	got = ioctl(*(int *) dev, USBDEVFS_BULK, &req);
	if (got < 0)
		return 0;
	wrote = write(1, buf, got);
	if (wrote < 0) {
		perror("write");
		exit(1);
	}
	if (wrote != got) {
		fprintf(stderr, "short write\n");
		exit(1);
	}
	return 1;
}


static void *usb_read_loop(void *arg)
{
	usb_dev_handle *dev = arg;

	while (synchronous_bulk_read(dev));
	disconnected = 1;
	return NULL;
}


static int poll_tty(usb_dev_handle *dev)
{
	static int escape = 0;
	char buf[100];
	int res, i;
	ssize_t got;

	got = read(0, buf, sizeof(buf));
	if (got < 0) {
		if (errno == EAGAIN) {
			usleep(10000);
			return 1;
		}
		perror("read");
		exit(1);
	}
	for (i = 0; i != got; i++)
		switch (buf[i]) {
		case '~':
			escape = 1;
			break;
		case '.':
			if (escape) {
				if (dev)
					usb_reset(dev);
				exit(0);
			}
			/* fall through */
		default:
			escape = 0;
		}
	if (!dev)
		return 1;
	res = usb_bulk_write(dev, 0x01, buf, got, 1000);
	if (res < 0)
		return 0;
	if (res != got) {
		fprintf(stderr, "short write\n");
		exit(1);
	}
	return 1;
}


static void poll_loop(usb_dev_handle *dev)
{
	pthread_t thread;

	disconnected = 0;
	if (pthread_create(&thread, NULL, usb_read_loop, dev) < 0) {
		perror("pthread_create");
		exit(1);
	}
	while (1) {
		if (disconnected)
			break;
		if (!poll_tty(dev))
			break;
	}
	pthread_kill(thread, SIGINT);
	pthread_join(thread, NULL);
}


static void usage(const char *name)
{
	fprintf(stderr, "usage: %s [-d vendor:product]\n", name);
	exit(1);
}


int main(int argc, char **argv)
{
	usb_dev_handle *dev;
	int c;
	int reported = 0;
	int res;

	while ((c = getopt(argc, argv, "d:")) != EOF)
		switch (c) {
		case 'd':
			parse_usb_id(optarg);
			break;
		default:
			usage(*argv);
		}

	make_raw(0, &console);
	atexit(cleanup);
	while (1) {
		dev = open_usb();
		if (dev) {
			res = usb_claim_interface(dev, 0);
			if (res >= 0) {
				fprintf(stderr, "[ Connected ]\r\n");
				reported = 0;
				poll_loop(dev);
			}
			usb_close(dev);
		}
		if (!reported)
			fprintf(stderr, "[ Waiting for device ]\r\n");
		reported = 1;
		poll_tty(NULL);
		usleep(100000);
	}
	return 0;
}
