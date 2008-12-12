CC=sdcc
CFLAGS=-I../common -DLOW_SPEED

MAIN=main
OBJS=$(MAIN) uart usb descr dfu version

.SUFFIXES:	.rel .ihx .bin

.PHONY:		test clean upload spotless version

all:		$(MAIN).bin

version:
		@if [ -f .version ]; then \
		    v=`cat .version`; \
		    expr $$v + 1 >.version; \
		else \
		    echo 0 >.version; \
		fi
		@[ -s .version ] || echo 0 >.version
		@echo '#include "version.h"' >version.c
		@echo "const char *build_date = \"`date`\";" >>version.c
		@echo "const uint16_t build_number = `cat .version`;" \
		  >>version.c

version.rel:	version

$(MAIN).ihx:	$(OBJS:%=%.rel)
		$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS:%=%.rel)

.ihx.bin:
		objcopy -I ihex $< -O binary $@
		@echo "build #`cat .version`, `ls -l $@`"

.rel.ihx:
		$(CC) $(CFLAGS) $<

.c.rel:
		$(CC) $(CFLAGS) -c $<


depend:
		>.depend
		touch version.h
		for n in *.c; do \
		    $(CPP) $(CFLAGS) -MM -MG $$n >>.depend || \
		      { rm -f .depend; exit 1; }; \
		done

ifeq (.depend,$(wildcard .depend))
include .depend
endif

clean:
		rm -f $(OBJS:%=%.rel) .depend
		rm -f $(OBJS:%=%.asm) $(OBJS:%=%.lst)
		rm -f $(OBJS:%=%.rst) $(OBJS:%=%.sym)
		rm -f $(MAIN).ihx $(MAIN).lnk $(MAIN).map $(MAIN).mem

spotless:	clean
		rm -f $(MAIN).bin

upload:
		ssh lab neo 'cat \>$(MAIN).bin' <$(MAIN).bin
