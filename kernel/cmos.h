#ifndef CMOS_H
#define CMOS_H

#include "kaos_int.h"

void    cmos_init();
uint8_t cmos_read_reg(uint8_t reg);
void    cmos_write_reg(uint8_t reg, uint8_t val);

#endif /* CMOS_H */
