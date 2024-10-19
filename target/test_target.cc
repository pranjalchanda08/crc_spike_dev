#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// MMIO Addresses
#define POLYNOMIAL_REG 0x1000
#define DATA_ADDRESS_REG 0x1008
#define LENGTH_REG 0x1010
#define COMMAND_REG 0x1018
#define RESULT_REG 0x1020
#define INTERRUPT_ENABLE_REG 0x1024

volatile int crc_done = 0;  // Flag to indicate interrupt received

// Interrupt Handler
void handle_interrupt() {
    printf("Interrupt received: CRC calculation complete!\n");
    crc_done = 1;
}

// Software CRC implementation for comparison
uint16_t software_crc16(uint8_t* data, size_t len, uint32_t poly) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i) {
        crc ^= (data[i] << 8);
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ poly;
            else
                crc <<= 1;
        }
    }
    return crc;
}

int main() {
    // Register the interrupt handler
    // (Platform-specific; pseudo-code for simplicity)
    register_interrupt_handler(handle_interrupt);

    // Generate pseudo-random test data
    uint8_t data[16];
    for (int i = 0; i < 16; ++i) {
        data[i] = rand() % 256;
    }

    // Configure polynomial
    uint32_t poly = 0x1021;
    *(volatile uint32_t*)POLYNOMIAL_REG = poly;

    // Provide data location and length
    *(volatile uint64_t*)DATA_ADDRESS_REG = (uint64_t)data;
    *(volatile uint32_t*)LENGTH_REG = 16;

    // Enable interrupt
    *(volatile uint32_t*)INTERRUPT_ENABLE_REG = 1;

    // Start CRC calculation
    *(volatile uint32_t*)COMMAND_REG = 1;

    // Wait for the interrupt to set the flag
    while (!crc_done);

    // Read hardware CRC result
    uint16_t hw_crc = *(volatile uint16_t*)RESULT_REG;

    // Calculate CRC in software for comparison
    uint16_t sw_crc = software_crc16(data, 16, poly);

    // Verify CRC match
    if (hw_crc == sw_crc) {
        printf("CRC Match! HW CRC: 0x%04X, SW CRC: 0x%04X\n", hw_crc, sw_crc);
    } else {
        printf("CRC Mismatch! HW CRC: 0x%04X, SW CRC: 0x%04X\n", hw_crc, sw_crc);
    }

    return 0;
}
