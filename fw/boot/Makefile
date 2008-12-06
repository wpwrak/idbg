CC=sdcc
CFLAGS=-I../common

OBJS=main.rel uart.rel

.SUFFIXES:	.rel .ihx .bin

.PHONY:		test clean upload spotless

all:		main.bin

main.bin:	$(OBJS)
		$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS)

.ihx.bin:
		objcopy -I ihex $< -O binary $@

.rel.ihx:
		$(CC) $(CFLAGS) $<

.c.rel:
		$(CC) $(CFLAGS) -c $<

clean:
		rm -f *.ihx *.asm *.lnk *.lst *.map *.mem *.rel *.rst *.sym

spotless:	clean
		rm -f *.bin

upload:
		ssh lab neo 'cat \>main.bin' <main.bin
