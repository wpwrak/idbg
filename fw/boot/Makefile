CC=sdcc
# SDCC doesn't like shell meta-characters in this define
CFLAGS=-I../common -DDATE='\"`date | tr \   _`\"'

MAIN=main
OBJS=$(MAIN).rel uart.rel usb.rel

.SUFFIXES:	.rel .ihx .bin

.PHONY:		test clean upload spotless $(MAIN).c

all:		$(MAIN).bin

$(MAIN).ihx:	$(OBJS)
		$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS)

.ihx.bin:
		objcopy -I ihex $< -O binary $@

.rel.ihx:
		$(CC) $(CFLAGS) $<

.c.rel:
		$(CC) $(CFLAGS) -c $<

clean:
		rm -f $(OBJS)
		rm -f $(OBJS:%.rel=%.asm) $(OBJS:%.rel=%.lst)
		rm -f $(OBJS:%.rel=%.rst) $(OBJS:%.rel=%.sym)
		rm -f $(MAIN).ihx $(MAIN).lnk $(MAIN).map $(MAIN).mem

spotless:	clean
		rm -f $(MAIN).bin

upload:
		ssh lab neo 'cat \>$(MAIN).bin' <$(MAIN).bin
