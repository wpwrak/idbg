#
# Written 2008-2010 by Werner Almesberger
# Copyright 2008-2010 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


ACCOUNT=werner@almesberger.net
DIR=/home/httpd/almesberger/misc/idbg.git/

DIRS=fw tools
TARGET_ONLY_DIRS=f326

.PHONY:		push

include Makefile.recurse

.PHONY:		gta ben-v1 ben_v1 ben-v2 ben_v2

push:
		git update-server-info
		rsync -av -e ssh .git/ $(ACCOUNT):$(DIR)

gta:
		$(MAKE) -C fw gta

ben-v1 ben_v1:
		$(MAKE) -C fw ben-v1

ben-v2 ben_v2:
		$(MAKE) -C fw ben-v2
