#
# idbg-gpio/Makefile - Build the GPIO utility
#
# Written 2008 by Werner Almesberger
# Copyright 2008 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


CFLAGS=-Wall -I../../fw/idbg
LDFLAGS=-lusb
MAIN=idbg-gpio

OBJS=

.PHONY:		all clean spotless

all:		$(MAIN)

clean:
		rm -f $(OBJS)

spotless:	clean
		rm -f $(MAIN)
