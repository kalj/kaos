#include "vga.h"

#include "portio.h"

 uint16_t vga_get_cursor_position() {
    uint16_t pos = 0;
    portio_outb(0x3D4, 0x0F);
    pos |= portio_inb(0x3D5);
    portio_outb(0x3D4, 0x0E);
    pos |= ((uint16_t)portio_inb(0x3D5)) << 8;
    return pos;
}

 void vga_set_cursor_position(uint16_t pos) {
    portio_outb(0x3D4, 0x0F);
    portio_outb(0x3D5, (uint8_t)(pos & 0xFF));
    portio_outb(0x3D4, 0x0E);
    portio_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}
