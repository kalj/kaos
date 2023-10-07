#ifndef VGA_H
#define VGA_H

#include "kaos_int.h"

#define VGA_MEMORY_BASE ((char *)0xB8000)

uint16_t vga_get_cursor_position();

void vga_set_cursor_position(uint16_t pos);

#endif /* VGA_H */
