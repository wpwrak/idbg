.PHONY:		all depend clean spotless

all:		io-parts.h

io-parts.h:	io.h Makefile
		awk \
		  'BEGIN { print "/* MACHINE-GENERATED. DO NOT EDIT ! */"; \
			print "#ifndef IO_PARTS_H"; \
			print "#define IO_PARTS_H"; } \
		   /#define/ && $$3 != "" { \
			split($$3, a, "_"); \
			print $$1, $$2 "_PORT", a[1]; \
			print $$1, $$2 "_MODE", a[1] "MDOUT"; \
			print $$1, $$2 "_BIT", a[2]; } \
		   END { print "#endif" }' <$< >$@ || \
			    { rm -f $@; exit 1; }


#
# When we do a global "make depend", we'll come here first. So we create
# io-parts.h so that "make depend" in the other directories can pick it up.
#

depend:		io-parts.h

clean:
		rm -f io-parts.h

spotless:	clean
