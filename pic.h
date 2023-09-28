#ifndef _PIC_H
#define _PIC_H

#include "kaos_int.h"

void pic_init();
void pic_set_mask(uint8_t pic1_mask, uint8_t pic2_mask);

void pic_eoi(int irq);

#endif /* _PIC_H */
