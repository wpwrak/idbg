#include <stdint.h>

#include "regs.h"
#include "uart.h"
#include "usb.h"


static void usb_write(uint8_t reg, uint8_t value)
{
    while (USB0ADR & BUSY);
    USB0ADR = reg;
    USB0DAT = value;
}


static uint8_t usb_read(uint8_t reg)
{
    while (USB0ADR & BUSY);
    USB0ADR = reg | BUSY;
    while (USB0ADR & BUSY);
    return USB0DAT;
}


static void poll(void)
{
    uint8_t n, i;

    while (!(usb_read(E0CSR) & OPRDY));
    print("OPRDY\n");
    n = usb_read(E0CNT);
    hex(n);
    for (i = 0; i != n; i++)
	hex(usb_read(FIFO0));
}


void usb_init(void)
{
print("usb_init\n");
    usb_write(POWER, USBRST | USBINH);	/* reset USB0 */
print("-2-\n");
    USB0XCN = PHYEN | SPEED;		/* configure and enable transceiver */
    USB0XCN = PHYEN ;		/* configure and enable transceiver */
print("-3-\n");
    usb_write(POWER, 0);		/* enable USB0 */
print("-4-\n");
//    usb_write(CLKREC, 0x09 | CRE);	/* enable USB clock recovery */
print("-1-\n");
    USB0XCN |= PREN;			/* enable pull-up */

#if 0
print("oscillating\n");
while (1) {
    USB0XCN |= PHYTST0 | PHYTST1;
    USB0XCN ^= PHYTST0;
    USB0XCN ^= PHYTST0 | PHYTST1;
}
#endif

    poll();
    USB0XCN &= ~PREN;			/* disable pull-up */
}
