#include "tty.h"

#include "portio.h"
#include "vga.h"
#include "memcpy.h"

#define N_COLUMNS 80
#define N_ROWS 25

static int cursor_position = 0;

void tty_init() {
    cursor_position = vga_get_cursor_position();
}

void tty_set_cursor_position(int row, int col) {
    cursor_position = N_COLUMNS * (row>=N_ROWS?N_ROWS-1:row) + (col>=N_COLUMNS?N_COLUMNS-1:col);
    vga_set_cursor_position(cursor_position);
}

void tty_clear_screen() {
    for (int i = 0; i < N_COLUMNS * N_ROWS; i++) {
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
    if(cursor_position == N_ROWS*N_COLUMNS)
    {
        /* scroll */
        for(int r=1; r<N_ROWS; r++)
        {
            memcpy(&VGA_MEMORY_BASE[2*(r-1)*N_COLUMNS], &VGA_MEMORY_BASE[2*r*N_COLUMNS], 2*N_COLUMNS);
        }
        // clear last row
        memset(&VGA_MEMORY_BASE[2*(N_ROWS-1)*N_COLUMNS], 0, 2*N_COLUMNS);
        cursor_position = N_COLUMNS*(N_ROWS-1);
    }
}

void tty_putc(char c) {
    print_char_impl(c, 0x07);
    vga_set_cursor_position(cursor_position);
}

void tty_puts(const char *msg) {
    while (*msg != 0) {
        print_char_impl(*msg, 0x07);
        msg++;
    }
    vga_set_cursor_position(cursor_position);
}
