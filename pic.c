#include "pic.h"

#include "portio.h"

#define PIC1         0x20 /* IO base address for master PIC */
#define PIC2         0xA0 /* IO base address for slave PIC */
#define PIC1_COMMAND PIC1
#define PIC1_DATA    (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA    (PIC2 + 1)

#define PIC_EOI 0x20

void pic_init()
{
    /* setup PIC */
    portio_outb(PIC1_COMMAND, 0x11); // start initialization with ICW4 present
    portio_wait();
    portio_outb(PIC2_COMMAND, 0x11);
    portio_wait();
    // ICW2 - remap offset address of idt_table
    portio_outb(PIC1_DATA, 0x20);
    portio_wait();
    portio_outb(PIC2_DATA, 0x28);
    portio_wait();

    /* ICW3 - setup cascading */
    portio_outb(PIC1_DATA, 0x04);
    portio_wait();
    portio_outb(PIC2_DATA, 0x02);
    portio_wait();

    // ICW4: have the PICs use 8086 mode (and not 8080 mode)
    portio_outb(PIC1_DATA, 0x1);
    portio_wait();
    portio_outb(PIC2_DATA, 0x1);
    portio_wait();
}
void pic_set_mask(uint8_t pic1_mask, uint8_t pic2_mask)
{
    portio_outb(PIC1_DATA, pic1_mask);
    portio_wait();
    portio_outb(PIC2_DATA, pic2_mask);
    portio_wait();
}

void pic_eoi(int irq)
{
    if(irq > 8)
        portio_outb(PIC2_COMMAND, PIC_EOI);
    else
        portio_outb(PIC1_COMMAND, PIC_EOI);
}
