#ifndef JTAG_H
#define JTAG_H

#include <stdint.h>


#define JTAG_MAX_BITS	255


enum jtag_gpios {
	JTAG_nTRST,
	JTAG_TMS,
	JTAG_TDO,
	JTAG_TDI,
	JTAG_TCK,
	JTAG_nSRST,
};


extern __xdata uint8_t jtag_data[JTAG_MAX_BITS/8];

uint8_t jtag_gpio_get(void);
void jtag_gpio_set(uint8_t set, uint8_t mask);

void jtag_send(const uint8_t *buf, uint8_t bits);

void jtag_attach(void);
void jtag_detach(void);

#endif /* !JTAG_H */
