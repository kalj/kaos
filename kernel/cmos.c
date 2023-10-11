#include "cmos.h"

#include "portio.h"

#define ADDRESS_PORT 0x70
#define DATA_PORT 0x71

static int nmi_disable_bit;

void cmos_init()
{
    nmi_disable_bit = (portio_inb(ADDRESS_PORT)>>7)&0x1;
}

uint8_t cmos_read_reg(uint8_t reg)
{
    portio_outb(ADDRESS_PORT, reg|(nmi_disable_bit<<7));

    portio_wait();

    return portio_inb(DATA_PORT);
}

void cmos_write_reg(uint8_t reg, uint8_t val)
{
    portio_outb(ADDRESS_PORT, reg|(nmi_disable_bit<<7));

    portio_wait();

    portio_outb(DATA_PORT, val);
}
