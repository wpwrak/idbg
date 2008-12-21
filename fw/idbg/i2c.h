#ifndef I2C_H
#define I2C_H

#include <stdint.h>


/* @@@ we need a bit better signaling for asynchronous operation */

bit i2c_write(uint8_t device, uint8_t addr, const uint8_t *buf, uint8_t len);
bit i2c_read(uint8_t device, uint8_t addr, uint8_t *buf, uint8_t len);

#endif /* !I2C_H */
