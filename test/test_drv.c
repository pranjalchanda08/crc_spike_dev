#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "crc_driver.h"

#define BUFFER_LEN  5
#define CRC_POLY    0x66736628

uint8_t buffer[BUFFER_LEN];

uint32_t software_crc32(uint8_t* data, size_t len, uint32_t poly) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ poly;
            else
                crc >>= 1;
        }
    }
    return crc ^ 0xFFFFFFFF;
}

int main()
{
    printf("*****STARTING Test******\n");

    /* Enable CRC device */
    crc_dev_en(1);
    
    /* Set HW CRC polynomial and polynomial type */
    crc_set_poly(CRC_POLY, CRC_POLYNOMIAL_TYPE_32);

    /* Gnerate 5 random numbers */
    for (size_t i = 0; i < BUFFER_LEN; i++)
        buffer[i] = rand();

    /* Set data length starts the conversion as well */
    crc_set_data((uint8_t*)&buffer, BUFFER_LEN);
    /* Wait for conversion complete */
    while (crc_is_busy());

    /* Get HW CRC results */
    uint32_t hw_crc = crc_get_res();

    /* Perform correspondig SW CRC calculations */
    uint32_t sw_crc = software_crc32((uint8_t*)&buffer, BUFFER_LEN, CRC_POLY);

    printf("HW CRC result: 0x%lx\n", hw_crc);
    printf("SW CRC result: 0x%lx\n", sw_crc);
    printf("TEST Result: %s\n", hw_crc == sw_crc ? "PASS" : "FAIL");

    printf("*****ENDING Test******\n");
    return 0;
    
}

