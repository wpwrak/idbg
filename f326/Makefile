#
# f326/Makefile - Build the C8051F326/7 Flash programmer
#
# Written 2008 by Werner Almesberger
# Copyright 2008 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


CC=arm-angstrom-linux-gnueabi-gcc

CFLAGS=-Wall -Wshadow -g -O
LDFLAGS=

PREFIX=/usr

NAME=f326
OBJS=f326.o flash.o c2.o gpio.o rt.o boundary.o

.PHONY:		all install uninstall clean depend spotless

all:		$(NAME)

$(NAME):	$(OBJS)

upload:
		ssh lab neo 'cat \>f326' <f326

install:	$(NAME)
		install -D $(NAME) $(PREFIX)/bin/$(NAME)

uninstall:
		rm -f $(PREFIX)/bin/$(NAME)

depend:
		$(CPP) $(CFLAGS) -MM -MG *.c >.depend || \
		  { rm -f .depend; exit 1; }

ifeq (.depend,$(wildcard .depend))
include .depend
endif

clean:
		rm -f $(OBJS) .depend

spotless:	clean
		rm -f $(NAME)
