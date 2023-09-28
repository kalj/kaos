#include "pic.h"
#include "portio.h"
#include "kaos.h"
#include "strfmt.h"

static char buf[10];

__attribute__((interrupt)) void keyboard_handler(void *irq_frame) {
    uint8_t status = portio_inb(0x64);
    /* Lowest bit of status will be set if buffer is not empty */
    if (status & 0x01) {
        uint8_t keycode = portio_inb(0x60);
        strfmt_u8_hex(buf,10,keycode);
        /* kaos_puts("0x"); */
        /* uint8_t nibble = (keycode >> 4) & 0xf; */
        /* char c         = nibble < 10 ? '0' + nibble : 'a' + (nibble - 10); */
        /* kaos_putc(c); */
        /* nibble = keycode & 0xf; */
        /* c      = nibble < 10 ? '0' + nibble : 'a' + (nibble - 10); */
        /* kaos_putc(c); */
        buf[2] = ' ';
        buf[3] = '\0';
        kaos_puts(buf);
    }
    pic_eoi(0); // incorrect but meh
}
