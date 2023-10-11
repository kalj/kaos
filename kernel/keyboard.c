#include "irq.h"
#include "kaos.h"
#include "pic.h"
#include "portio.h"
#include "strfmt.h"

#define IRQ_VEC_KEYBOARD 33

static char buf[10];

__attribute__((interrupt)) void keyboard_handler(void *irq_frame)
{
    uint8_t status = portio_inb(0x64);
    /* Lowest bit of status will be set if buffer is not empty */
    if (status & 0x01) {
        uint8_t keycode = portio_inb(0x60);
        strfmt_u8_hex(buf, 10, keycode);
        buf[2] = ' ';
        buf[3] = '\0';
        kaos_puts(buf);
    }
    pic_eoi(PIC_IRQ_KEYBOARD);
}

void keyboard_init()
{
    pic_enable_interrupt(PIC_IRQ_KEYBOARD);
    irq_register_handler(IRQ_VEC_KEYBOARD, keyboard_handler, 0x8E);
}