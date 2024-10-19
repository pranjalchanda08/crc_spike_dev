#ifndef _CRC_DRIVER_H_
#define _CRC_DRIVER_H_

#include <inttypes.h>

typedef uint8_t bool;

#define CRC_DEV_MMIO_BASE 0x4535FF00

#define MMIO_CRC_CR CRC_DEV_MMIO_BASE + 0x0            // W Access to Control Register
#define MMIO_CRC_SR CRC_DEV_MMIO_BASE + 0x4            // R/W Access Status register; only intr flag is writable to clear it post serving of interrupt
#define MMIO_CRC_RESULT CRC_DEV_MMIO_BASE + 0x8        // R Access Result Register
#define MMIO_CRC_SET_POLY CRC_DEV_MMIO_BASE + 0xC      // R/W Save u32_polynomial value
#define MMIO_CRC_DATA CRC_DEV_MMIO_BASE + 0x10         // W Access Data pointer Register
#define MMIO_CRC_SET_DATA_LEN CRC_DEV_MMIO_BASE + 0x14 // R/W Save u32_data_len value

#define CRC_EN_MASK (1 << 0)
#define CRC_BUSY_MASK (1 << 2)

void crc_set_poly(uint32_t poly);
void crc_get_poly(uint32_t *poly);
void crc_dev_en(bool en);
uint32_t crc_get_res();
void crc_set_data_ptr(uint8_t *data_ptr);
void crc_set_data_len(uint32_t data_len);
bool crc_is_busy();

#endif /* _CRC_DRIVER_H_ */
