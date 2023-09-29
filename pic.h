#ifndef _PIC_H
#define _PIC_H

#include "kaos_int.h"

#define PIC_IRQ_KEYBOARD 1

void pic_init();

int pic_get_interrupt_state(int irq);
void pic_enable_interrupt(int irq);
void pic_disable_interrupt(int irq);

void pic_eoi(int irq);

#endif /* _PIC_H */
