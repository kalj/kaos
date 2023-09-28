#ifndef TTY_H
#define TTY_H

void tty_init();

void tty_set_cursor_position(int row, int col);

void tty_clear_screen();

void tty_putc(char c);

void tty_puts(const char *msg);

#endif /* TTY_H */
