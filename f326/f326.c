#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "c2.h"
#include "flash.h"


static void dump(const char *title, void *data, size_t size)
{
    int i, j;

    fprintf(stderr, "%s:\n", title);
    for (i = 0; i < size; i += 16) {
	fprintf(stderr, "  %04x", i);
	for (j = 0; j != 16 && i+j < size; j++)
	    fprintf(stderr, " %02x", ((uint8_t *) data)[i+j]);
	fprintf(stderr, "\n");
    }
}


static void flash_device(void *data, size_t size)
{
    int i;
    size_t len;
    uint8_t buf[256];

    for (i = 0; i < size; i += 256)
	fputc('-', stderr);
    fputc('\r', stderr);

    flash_init();
    flash_device_erase();

    for (i = 0; i < size; i += 256) {
	fputc('*', stderr);
	fflush(stderr);
	len = size-i <= 256 ? size-i : 256;
	flash_block_write(i, data+i, len);
    }
    fputc('\r', stderr);

    for (i = 0; i < size; i += 256) {
	fputc('#', stderr);
	fflush(stderr);
	len = size-i <= 256 ? size-i : 256;
	flash_block_read(i, buf, len);
	if (memcmp(buf, data+i, len)) {
	    fprintf(stderr, "compare error at 0x%04x\n", i);
	    dump("Expected", data+i, len);
	    dump("Read", buf, len);
	    exit(1);
	}
    }
    fputc('\n', stderr);
}


static void erase_flash(void)
{
    flash_init();
    flash_device_erase();
}


static void dump_flash(size_t size)
{
    int i, j;
    size_t len;
    uint8_t buf[256], last[256];
    int skipping = 0;

    flash_init();
    for (i = 0; i < size; i += 16) {
	len = size-i <= 16 ? size-i : 16;
	flash_block_read(i, buf, len);
	if (i && !memcmp(last, buf, len)) {
	    printf("%04x: *%c", i, skipping ? '\r' : '\n');
	    fflush(stdout);
	    skipping = 1;
	    continue;
	}
	skipping = 0;
	memcpy(last, buf, len);
	printf("%04x:", i);
	for (j = 0; j != len; j++)
	    printf(" %02x", buf[j]);
	printf("  ");
	for (j = 0; j != len; j++)
	    printf("%c", buf[j] >= ' ' && buf[j] <= '~' ? buf[j] : '.');
	putchar('\n');
	fflush(stdout);
    }
}


static void identify(void)
{
    int i;

    c2_addr_write(0);
    printf("Dev");
    for (i = 0; i != 10; i++)
	printf(" 0x%02x", c2_data_read(1));
    c2_addr_write(1);
    printf("\nRev");
    for (i = 0; i != 10; i++)
	printf(" 0x%02x", c2_data_read(1));
    printf("\n");
}


static void do_flash(const char *name)
{
    FILE *file;
    uint8_t code[16384];
    size_t size;

    file = fopen(name, "r");
    if (!file) {
	perror(name);
	exit(1);
    }
    size = fread(code, 1, sizeof(code), file);
    (void) fclose(file);
    flash_device(code, size);
}


static void usage(const char *name)
{
    fprintf(stderr,
"usage: %s [-d|-e|file]\n\n"
"  -d  dump Flash content\n"
"  -e  erase whole Flash\n\n"
"No argument resets the F326.\n"
  , name);
    exit(1);
}


int main(int argc, char **argv)
{
    c2_init();
    identify();

    if (argc > 2)
	usage(*argv);
    if (argc == 2) {
	if (!strcmp(argv[1], "-d"))	
	    dump_flash(0x4000);
	else if (!strcmp(argv[1], "-e"))	
	    erase_flash();
	else if (*argv[1] == '-')
	    usage(*argv);
	else {
	    do_flash(argv[1]);
	    identify();
	}
    }
    c2_reset();

    return 0;
}
