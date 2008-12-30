#
# idbg-con/Makefile - Build the IDBG console
#
# Written 2008 by Werner Almesberger
# Copyright 2008 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


CFLAGS=-Wall
LDFLAGS=-lusb
MAIN=idbg-con

OBJS=

.PHONY:		all clean spotless

all:		$(MAIN)

clean:
		rm -f $(OBJS)

spotless:	clean
		rm -f $(MAIN)
