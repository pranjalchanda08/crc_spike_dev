/***********************************************************************************
 * @author: pranjalchanda08@gmail.com [Pranjal Chanda]
 * @file: crc_dev.cc
 * @brief:  Definations of CRC Device
 **********************************************************************************/

/**********************************************************************************
 * INCLUDES
 **********************************************************************************/
#include "crc_dev.h"
#include <iostream>
#include "stdio.h"

#define PTR_ACCESS(_x) *(uint32_t *)_x

/**********************************************************************************
 * PUBLIC FUNCTION DEFINATION
 **********************************************************************************/
crc_dev_t::crc_dev_t()
{
    std::cout << "Initialising CRC" << std::endl;
}

crc_dev_t::crc_dev_t(abstract_interrupt_controller_t *intctrl, reg_t int_id) : u8p_data(0), interrupt_id(int_id), intctrl(intctrl)
{
    crc_dev_t();
}

crc_dev_t ::crc_dev_t(sim_t *sim, abstract_interrupt_controller_t *intctrl, reg_t int_id) : sim_ptr(sim)
{
    crc_dev_t(intctrl, int_id);
}

bool crc_dev_t::load(reg_t addr, size_t len, uint8_t *bytes)
{
    printf("[D]: CRC Load: 0x%lx\n", addr);
    uint32_t dummy;
    /* Make sure the buffer length of size crc_t */
    if (len != sizeof(crc_t))
        return false;
    switch (addr)
    {
    case MMIO_CRC_RESULT:
        dummy = u32_crc_res;
        break;
    case MMIO_CRC_CR:
        dummy = csr_u.csr_r & 0xFFFF;
        break;
    case MMIO_CRC_SR:
        dummy = (csr_u.csr_r >> 16) & 0xFFFF;
        break;
    case MMIO_CRC_SET_POLY:
        dummy = u32_polynomial;
    case MMIO_CRC_SET_DATA_LEN:
    case MMIO_CRC_DATA:
        /* Read as Zero (RAZ) */
        dummy = 0;
        break;
    default:
        return false;
    }
    memcpy(bytes, &dummy, sizeof(uint32_t));
    return true;
}

bool crc_dev_t::store(reg_t addr, size_t len, const uint8_t *bytes)
{
    printf("[D]: CRC Store: 0x%lx 0x%x\n", addr, PTR_ACCESS(bytes));
    crc_t dummy;
    switch (addr)
    {
    case MMIO_CRC_CR:
        dummy = PTR_ACCESS(bytes);
        csr_u.csr_r = dummy & 0xFFFF; // Write lower 16 bits of control register
        break;
    case MMIO_CRC_SET_POLY:
        /* Copy polynomial value to polynomial register */
        u32_polynomial = PTR_ACCESS(bytes);
        break;
    case MMIO_CRC_DATA:
        /* Copy the address of the buffer pointer */
        u8p_data = PTR_ACCESS(bytes);
        break;
    case MMIO_CRC_SET_DATA_LEN:
        /* Set data length */
        u32_data_length = PTR_ACCESS(bytes);
        return hw_crc_convert();
    default:
        return false;
    }
    return true;
}

void crc_dev_t::tick(reg_t UNUSED rtc_ticks)
{
    return;
}

/**********************************************************************************
 * PRIVATE FUNCTION DEFINATION
 **********************************************************************************/
/**
 * @brief  Perform the hw crc_conversion as per polynomial type
 */
bool crc_dev_t::hw_crc_convert()
{
    crc_t crc = UINT32_MAX;
    crc_t crc_width;

    if (!csr_u.csr_s.c_en || csr_u.csr_s.s_busy)
    {
        /* return if not enable or already busy */
        printf("[D]: hw_crc_convert fail: %d, %d\n", csr_u.csr_s.c_en, csr_u.csr_s.s_busy);
        return false;
    }
    /* Raise the busy flag as we will start the conversion */
    csr_u.csr_s.s_busy = true;

    switch (csr_u.csr_s.c_poly_type)
    {
    case CRC_POLYNOMIAL_TYPE_32:
        crc = UINT32_MAX;
        crc_width = 32;
        break;
    case CRC_POLYNOMIAL_TYPE_16:
        crc = UINT16_MAX;
        crc_width = 16;
        break;
    case CRC_POLYNOMIAL_TYPE_8:
        crc = 0x00;
        crc_width = 8;
        break;
    default:
        printf("[D]: hw_crc_convert default %d\n", csr_u.csr_s.c_poly_type);
        return false;
    }

    uint8_t temp;
    uint32_t temp_reg = u8p_data;

    for (size_t i = 0; i < u32_data_length; i++)
    {
        sim_ptr->mmio_load(temp_reg++, 1, &temp);
        crc ^= temp; // Align input byte with CRC size
        for (int j = 0; j < 8; j++)
        {
            if (crc & 1) // Check lowest bit
                crc = (crc >> 1) ^ u32_polynomial;
            else
                crc >>= 1;
        }
    }
    crc ^= ((1UL << crc_width) - 1); // Keep CRC constrained to its size

    u32_crc_res = crc;
    printf("[D]: hw_crc_convert crc: %x\n", crc);
    /* Release busy flag */
    csr_u.csr_s.s_busy = false;
    /* Raise interrupt */
    if (!csr_u.csr_s.s_int)
    {
        csr_u.csr_s.s_int = true;
        raise_intr();
    }
    else
    {
        /* Race condition */
    }
    return true;
}

void crc_dev_t::raise_intr()
{
    /* TODO */
}

int fdt_parse_crc_dev(const void *fdt, reg_t *crc_dev_addr,
                      const char *compatible)
{
    int nodeoffset, len, rc;
    const fdt32_t *reg_p;

    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
    if (nodeoffset < 0)
        return nodeoffset;

    rc = fdt_get_node_addr_size(fdt, nodeoffset, crc_dev_addr, NULL, "reg");
    if (rc < 0 || !crc_dev_addr)
        return -ENODEV;

    return 0;
}

// This function parses an instance of this device from the FDT
// An FDT node for a device should encode, at minimum, the base address for the device
crc_dev_t *crc_dev_parse_from_fdt(const void *fdt, const sim_t *sim, reg_t *base, std::vector<std::string> sargs)
{
    if (fdt_parse_crc_dev(fdt, base, "crc_dev,crc0") == 0)
    {
        printf("Found crc at %lx\n", *base);
        return new crc_dev_t((sim_t *)sim, sim->get_intctrl(), 2);
    }
    else
    {
        return nullptr;
    }
}

// This function is used to provide a default DTS node for spike's default DTS
// generator. This UART should not be generated by default, since it would
// conflict with the builtin ns16550 UART.
std::string crc_dev_generate_dts(const sim_t *sim, const std::vector<std::string> &args)
{
    return std::string();
}

/*
class crc_dev_factory_t : public device_factory_t
{
public:
    crc_dev_factory_t()
    {
        std::string str("crc_dev");
        if (!mmio_device_map().emplace(str, this).second)
            throw std::runtime_error("Plugin \"" + str + "\" already registered");
    };
    crc_dev_t *parse_from_fdt(const void *fdt, const sim_t *sim, reg_t *base, const std::vector<std::string> &sargs) const override { return crc_dev_parse_from_fdt(fdt, sim, base, sargs); }
    std::string generate_dts(const sim_t *sim, const std::vector<std::string> &sargs) const override { return crc_dev_generate_dts(sim, sargs); }
};
device_factory_t *crc_dev_factory = new crc_dev_factory_t();

*/
REGISTER_DEVICE(crc_dev, crc_dev_parse_from_fdt, crc_dev_generate_dts);
