#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "c2.h"


#define	P0	0x80
#define	P0MDOUT	0xa4
#define	P2	0xa0
#define	P2MDOUT	0xa6


static uint8_t reg_read(uint8_t addr)
{
    c2_addr_write(addr);
    return c2_data_read(1);
}


static void reg_write(uint8_t addr, uint8_t value)
{
    c2_addr_write(addr);
    c2_data_write(value, 1);
}


static const char *parse(const char *s, uint8_t *port, uint8_t *mode, int bits)
{
    int pos;

    *port = *mode = 0;
    pos = 0;
    while (pos != bits) {
	switch (*s++) {
	case '.':
	    continue;
	case '1':
	    *port |= 1 << pos;
	    /* fall through */
	case '0':
	    *mode |= 1 << pos;
	    break;
	case 'r':
	case 'R':
	    *port |= 1 << pos;
	    break;
	case 0:
	    fprintf(stderr, "not enough pin settings\n");
	    exit(1);
	default:
	    fprintf(stderr, "unrecognized pin setting \"%c\"\n", s[-1]);
	    exit(1);
	}
	pos++;
    }
    return s;
}


static void setup(const char *init)
{
    uint8_t p0, p0mdout, p2, p2mdout;

    init = parse(init, &p0, &p0mdout, 8);
    init = parse(init, &p2, &p2mdout, 6);
    if (*init) {
	fprintf(stderr, "too many pin settings\n");
	exit(1);
    }
    reg_write(P0, p0);
    reg_write(P0MDOUT, p0mdout);
    reg_write(P2, p2);
    reg_write(P2MDOUT, p2mdout);
}


static void print(uint8_t v, int bits)
{
    int pos;

    for (pos = 0; pos != bits; pos++)
	putchar(v & (1 << pos) ? '1' : '0');
}


static void scan(void)
{
    uint8_t p0, p2;

    p0 = reg_read(P0);
    p2 = reg_read(P2);
    print(p0, 8);
    putchar('.');
    print(p2, 6);
    putchar('\n');
}


static void __attribute__((unused)) dump(void)
{
    fprintf(stderr, "GPIOCN %02x\n", reg_read(0xe2));
    fprintf(stderr, "  P0      %02x\n", reg_read(P0));
    fprintf(stderr, "  P0MDOUT %02x\n", reg_read(P0MDOUT));
    fprintf(stderr, "  P2      %02x\n", reg_read(P2));
    fprintf(stderr, "  P2MDOUT %02x\n", reg_read(P2MDOUT));
}


void boundary(const char *init)
{
//dump();
    setup(init);
    scan();
}
