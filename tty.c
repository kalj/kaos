#include "tty.h"

#include "port_io.h"
#include "vga.h"

#define N_COLUMNS 80

static int cursor_position = 0;

static uint16_t get_cursor_position() {
    uint16_t pos = 0;
    outportb(0x3D4, 0x0F);
    pos |= inportb(0x3D5);
    outportb(0x3D4, 0x0E);
    pos |= ((uint16_t)inportb(0x3D5)) << 8;
    return pos;
}

static void set_cursor_position(uint16_t pos) {
    outportb(0x3D4, 0x0F);
    outportb(0x3D5, (uint8_t)(pos & 0xFF));
    outportb(0x3D4, 0x0E);
    outportb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void tty_init() {
    cursor_position = get_cursor_position();
}

void tty_set_cursor_position(int row, int col) {
    cursor_position = N_COLUMNS * row + col;
    set_cursor_position(cursor_position);
}

void tty_clear_screen() {
    for (int i = 0; i < N_COLUMNS * 20; i++) {
        VGA_MEMORY_BASE[i * 2] = 0;
        VGA_MEMORY_BASE[i * 2 + 1] = 0;
    }
}

static void print_char_impl(char c, char style) {
    if (c == '\n') {
        cursor_position = (cursor_position / N_COLUMNS + 1) * N_COLUMNS;
    } else {
        VGA_MEMORY_BASE[cursor_position * 2] = c;
        VGA_MEMORY_BASE[cursor_position * 2 + 1] = style;
        cursor_position++;
    }
}

void tty_print_char(char c) {
    print_char_impl(c, 0x07);
    set_cursor_position(cursor_position);
}

void tty_print_str(const char *msg) {
    while (*msg != 0) {
        print_char_impl(*msg, 0x07);
        msg++;
    }
    set_cursor_position(cursor_position);
}
