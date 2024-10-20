#include "crc_driver.h"

#define MMIO32(_x) (*(volatile uint32_t *)(_x))

void crc_set_poly(uint32_t poly, crc_poly_t type)
{
    MMIO32(MMIO_CRC_SET_POLY) = poly;
    uint16_t cr = MMIO32(MMIO_CRC_CR) & ~((uint32_t)0x6);
    cr |= ((type & 0x03) << 1);
    MMIO32(MMIO_CRC_CR) = cr;
}

void crc_get_poly(uint32_t *poly)
{
    *poly = MMIO32(MMIO_CRC_SET_POLY);
}

void crc_dev_en(bool en)
{
    uint16_t cr = MMIO32(MMIO_CRC_CR);
    if (en)
        cr |= CRC_EN_MASK;
    else
        cr &= ~CRC_EN_MASK;
    MMIO32(MMIO_CRC_CR) = cr;
}

uint32_t crc_get_res()
{
    return MMIO32(MMIO_CRC_RESULT);
}

void crc_set_data(uint8_t *data_ptr, uint32_t len)
{
    MMIO32(MMIO_CRC_DATA) = (uint32_t)(uintptr_t)data_ptr;
    MMIO32(MMIO_CRC_SET_DATA_LEN) = len;
}
bool crc_is_busy()
{
    return (MMIO32(MMIO_CRC_SR) & CRC_BUSY_MASK) != 0;
}