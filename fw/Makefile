#
# fw/Makefile - IDBG firmware build
#
# Written 2008, 2009 by Werner Almesberger
# Copyright 2008, 2009 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


DIRS=common boot idbg


.PHONY:		all depend install uninstall clean spotless

all:
		for d in $(DIRS); do make -C $$d all || exit 1; done

depend:
		for d in $(DIRS); do make -C $$d depend || exit 1; done

# Top-level Makefile recurses for "install" and "uninstall", which have no use
# here. Just ignore them.

install:

uninstall:

clean:
		for d in $(DIRS); do make -C $$d clean || exit 1; done

spotless:
		for d in $(DIRS); do make -C $$d spotless || exit 1; done
