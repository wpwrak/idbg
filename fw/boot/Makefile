#
# boot/Makefile - Makefile for DFU-capable boot loader
#
# Written 2008 by Werner Almesberger
# Copyright 2008 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

MAIN=boot
OBJS=$(MAIN) uart usb dfu version

include ../common/Makefile.system
include ../common/Makefile.common

CFLAGS += -I../include # -DLOW_SPEED
LDFLAGS += --code-size $(PAYLOAD_START)

uart.rel:	../common/uart.c
		$(CC) $(CFLAGS) -o $@ -c $<

usb.rel:	../common/usb.c
		$(CC) $(CFLAGS) -o $@ -c $<
