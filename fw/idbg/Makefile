#
# idbg/Makefile - Makefile for IDBG payload
#
# Written 2008-2010 by Werner Almesberger
# Copyright 2008-2010 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


MAIN = idbg

OBJS = $(MAIN) uart usb descr version ep0 serial
ifeq ($(shell cat ../.target),GTA)
OBJS += jtag i2c
endif

include ../common/Makefile.system
include ../common/Makefile.common

CFLAGS += -I../include # -DLOW_SPEED
LDFLAGS += --code-size $(PAYLOAD_SIZE) --code-loc $(PAYLOAD_START)

.PHONY:		dfu dfu-reset

uart.rel:	../common/uart.c
		$(CC) $(CFLAGS) $(DEFINE_UART_SPEED) -o $@ -c $<

usb.rel:	../common/usb.c
		$(CC) $(CFLAGS) -o $@ -c $<

dfu:
		dfu-util -d 0x1d50:0x1db5 -D idbg.bin

dfu-reset:
		dfu-util -d 0x1d50:0x1db5 -R -D idbg.bin
