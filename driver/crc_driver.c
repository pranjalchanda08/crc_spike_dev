/***********************************************************************************
 * @author: pranjalchanda08@gmail.com [Pranjal Chanda]
 * @file: crc_driver.c
 * @brief:  Interface driver defination for CRC Device
 **********************************************************************************/

/**********************************************************************************
 * INCLUDES
 **********************************************************************************/
#include "crc_driver.h"
/**********************************************************************************
 * DEFINES
 **********************************************************************************/
#define MMIO32(_x) (*(volatile uint32_t *)(_x))

/**********************************************************************************
 * DEINATIONS
 **********************************************************************************/
/**
 * @brief: Set CRC Polynomial and polynomial type
 * @param[in]   poly    Polynomial value
 * @param[in]   type    Polynomial type
 */
void crc_set_poly(uint32_t poly, crc_poly_t type)
{
    MMIO32(MMIO_CRC_SET_POLY) = poly;
    uint16_t cr = MMIO32(MMIO_CRC_CR) & ~((uint32_t)0x6);
    cr |= ((type & 0x03) << 1);
    MMIO32(MMIO_CRC_CR) = cr;
}

/**
 * @brief: Get CRC Polynomial and polynomial type
 * @param[out]   poly    Pointer to read Polynomial value
 */
void crc_get_poly(uint32_t *poly)
{
    *poly = MMIO32(MMIO_CRC_SET_POLY);
}

/**
 * @brief: Enable CRC device
 * @param[in]   en   1->enable; 0->disable
 */
void crc_dev_en(bool en)
{
    uint16_t cr = MMIO32(MMIO_CRC_CR);
    if (en)
        cr |= CRC_EN_MASK;
    else
        cr &= ~CRC_EN_MASK;
    MMIO32(MMIO_CRC_CR) = cr;
}

/**
 * @brief: Get CRC calculated result
 * @return uint32_t result
 */
uint32_t crc_get_res()
{
    return MMIO32(MMIO_CRC_RESULT);
}

/**
 * @brief: Set CRC Data buffer pointer and length of buffer
 * @param[in]   data_ptr    Pointer to data_buffer
 * @param[in]   len         Length of the data buffer
 */
void crc_set_data(uint8_t *data_ptr, uint32_t len)
{
    MMIO32(MMIO_CRC_DATA) = (uint32_t)(uintptr_t)data_ptr;
    MMIO32(MMIO_CRC_SET_DATA_LEN) = len;
}

/**
 * @brief: Is CRC engine busy
 * @return true if CRC busy flag is high
 */
bool crc_is_busy()
{
    return (MMIO32(MMIO_CRC_SR) & CRC_BUSY_MASK) != 0;
}