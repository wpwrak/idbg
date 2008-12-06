/*
 * gpio.c - Really primitive S3C244x GPIO access. Only works for ports B-H.
 */


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>


#define BASE 0x56000000


volatile void *mem;


void gpio_init(void)
{
	int fd;

	fd = open("/dev/mem", O_RDWR);
        if (fd < 0) {
		perror("/dev/mem");
		exit(1);
	}
	mem = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE);
	if (mem == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
}
