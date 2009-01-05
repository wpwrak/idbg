#
# idbg/Makefile - Makefile for IDBG payload
#
# Written 2008, 2009 by Werner Almesberger
# Copyright 2008, 2009 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


MAIN=idbg
OBJS=$(MAIN) uart usb descr version ep0 serial jtag i2c

include ../common/Makefile.system
include ../common/Makefile.common

CFLAGS += # -DLOW_SPEED
LDFLAGS += --code-size $(PAYLOAD_SIZE) --code-loc $(PAYLOAD_START)

uart.rel:	../common/uart.c
		$(CC) $(CFLAGS) -o $@ -c $<

usb.rel:	../common/usb.c
		$(CC) $(CFLAGS) -o $@ -c $<
