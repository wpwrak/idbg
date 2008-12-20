MAIN=idbg
OBJS=$(MAIN) uart usb descr

include ../common/Makefile.system
include ../common/Makefile.common

CFLAGS += -I../boot # -DLOW_SPEED
LDFLAGS += --code-size $(PAYLOAD_SIZE) --code-loc $(PAYLOAD_START)

uart.rel:	../boot/uart.c
		$(CC) $(CFLAGS) -o $@ -c $<

usb.rel:	../boot/usb.c
		$(CC) $(CFLAGS) -o $@ -c $<
