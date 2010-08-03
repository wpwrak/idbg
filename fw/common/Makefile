#
# common/Makefile - Makefile for shared items
#
# Written 2008, 2010 by Werner Almesberger
# Copyright 2008, 2010 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


GEN_quiet	= @echo "  GENERATE " $@ &&

ifeq ($(V),1)
    GEN		=
else
    GEN		= $(GEN_quiet)
endif


.PHONY:		all depend clean spotless

all:		io-parts.h

io-parts.h:	io.h Makefile ../.target
		$(GEN) $(CPP) -D`cat ../.target` -dD $< | \
		  sed '1,/IO_H/d' | \
		  awk \
		  'BEGIN { print "/* MACHINE-GENERATED. DO NOT EDIT ! */"; \
			print "#ifndef IO_PARTS_H"; \
			print "#define IO_PARTS_H"; } \
		   /#define/ && $$3 != "" { \
			split($$3, a, "_"); \
			print $$1, $$2 "_PORT", a[1]; \
			print $$1, $$2 "_MODE", a[1] "MDOUT"; \
			print $$1, $$2 "_BIT", a[2]; } \
		   END { print "#endif" }' >$@ || \
			    { rm -f $@; exit 1; }


#
# When we do a global "make depend", we'll come here first. So we create
# io-parts.h so that "make depend" in the other directories can pick it up.
#

depend:		io-parts.h

clean:
		rm -f io-parts.h

spotless:	clean
