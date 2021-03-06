#include <stdint.h>
#include <stdio.h>
#include <string.h>


#define __interrupt(x)


#define SERIAL_H
#define SERIAL_BUF_SIZE 2

#include "../../serial.c"


static void (*sim_callback)(void *user);
static void *sim_user, *sim_buf;


void __usb_send(const char *label, uint8_t *buf, int len,
    void (*cb)(void *user), void *user)
{
	int i;

	fprintf(stderr, "%s: %p %d\n", label, buf, len);
	for (i = 0; i != len; i++)
		fprintf(stderr, " %02x", buf[i]);
	fputc('\n', stderr);
	sim_callback = cb;
	sim_user = user;
}


void __usb_recv(const char *label, uint8_t *buf, int len,
    void (*cb)(void *user), void *user)
{
	fprintf(stderr, "%s: %p %d\n", label, buf, len);
	sim_callback = cb;
	sim_user = user;
	sim_buf = buf;
}


static void callback(void)
{
	sim_callback(sim_user);
}


static void usb_arrive(void *buf)
{
	fprintf(stderr, "usb_arrive \"%s\"\n", (char *) buf);
	memcpy(sim_buf, buf, strlen(buf));
	callback();
}


static void rx(char c)
{
	SBUF0 = c;
	RI0 = 1;
fprintf(stderr, "RX %c\n", c);
	uart_isr();
	RI0 = 0;
}


static void tx(void)
{
	SBUF0 = 0;
	serial_poll();
	if (SBUF0)
		fprintf(stderr, "tx %c\n", SBUF0);
	else
		fprintf(stderr, "----\n");
}


static void tx_done(void)
{
	TI0 = 1;
}


static void txn(int n)
{
	while (n--) {
		tx();
		tx();
		tx_done();
	}
}


int main(void)
{
	serial_init();
	serial_poll();

	/*
	 * The simulator only has one endpoint, so we do the TX test first.
	 */

	/* TX test */
	usb_arrive("TEST");
	usb_arrive("MORE");
	txn(4);
	txn(5);
	usb_arrive("XXXX");
	usb_arrive("YYYY");
	txn(10);

	/* RX test */

	rx('A');
	serial_poll();
	rx('B');
	rx('C');
	rx('D');
	rx('E');
	rx('F');
	rx('G');
	serial_poll();
	serial_poll();
	callback();
	serial_poll();
	serial_poll();
	callback();
	rx('H');
	serial_poll();
	serial_poll();
	callback();
	serial_poll();
	serial_poll();

	return 0;
}
