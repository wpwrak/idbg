#
# Written 2008, 2009 by Werner Almesberger
# Copyright 2008, 2009 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


ACCOUNT=werner@almesberger.net
DIR=/home/httpd/almesberger/misc/idbg.git/

DIRS=fw tools

.PHONY:		push

include Makefile.recurse

push:
		git-update-server-info
		rsync -av -e ssh .git/ $(ACCOUNT):$(DIR)
