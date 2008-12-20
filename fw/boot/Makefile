MAIN=boot
OBJS=$(MAIN) uart usb dfu version

include ../common/Makefile.system
include ../common/Makefile.common

CFLAGS += # -DLOW_SPEED
LDFLAGS += --code-size $(PAYLOAD_START)

uart.rel:	../common/uart.c
		$(CC) $(CFLAGS) -o $@ -c $<

usb.rel:	../common/usb.c
		$(CC) $(CFLAGS) -o $@ -c $<
