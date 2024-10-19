#ifndef _CRC_DEV_H_
#define _CRC_DEV_H_

#include <riscv/abstract_device.h>
#include <riscv/dts.h>
#include <riscv/sim.h>
#include <fesvr/term.h>
#include <fdt/libfdt.h>

#define CRC_DEV_MMIO_BASE 0x4535FF00

#define MMIO_CRC_CSR    CRC_DEV_MMIO_BASE + 0   // Access CSR Register
#define MMIO_CRC_DATA   CRC_DEV_MMIO_BASE + 1   // Access Data pointer Register
#define MMIO_CRC_RESULT CRC_DEV_MMIO_BASE + 2   // Access Result Register

typedef enum
{
    CRC_POLYNOMIAL_TYPE_32 = 0,
    CRC_POLYNOMIAL_TYPE_16,
    CRC_POLYNOMIAL_TYPE_8,
} crc_poly_t;

typedef struct crc_dev_csr
{
    uint32_t c_en : 1;
    uint32_t c_poly_type : 2; // represented by crc_poly_t
    uint32_t s_busy : 1;
} crc_dev_csr_t;

class crc_dev_t : public abstract_device_t
{
private:
    uint32_t polynomial;
    uint8_t *data;
    uint32_t data_size;
    union crc_csr_u
    {
        uint32_t csr_r;
        crc_dev_csr_t csr_s;
    } csr_u;

public:
    crc_dev_t() : data(0) {}
    bool load(reg_t addr, size_t len, uint8_t *bytes) override;
    bool store(reg_t addr, size_t len, const uint8_t *bytes) override;
    void tick(reg_t UNUSED rtc_ticks) override;
};

#endif /* _CRC_DEV_H_ */