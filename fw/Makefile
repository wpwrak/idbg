#
# fw/Makefile - IDBG firmware build
#
# Written 2008-2010 by Werner Almesberger
# Copyright 2008-2010 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


DIRS=common boot idbg


.PHONY:		all depend install uninstall clean spotless
.PHONY:		target gta ben-v1 ben_v1 ben-v2 ben_v2

all:		.target
		for d in $(DIRS); do make -C $$d all || exit 1; done

.target target:
		@echo "Please run  make gta|ben-v1|ben-v2" 2>&1
		@exit 1

gta:
		echo GTA >.target

ben-v1 ben_v1:
		echo BEN_V1 >.target

ben-v2 ben_v2:
		echo BEN_V2 >.target

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
		rm -f .target
