#ifndef TTY_H
#define TTY_H

void tty_init();

void tty_set_cursor_position(int row, int col);

void tty_clear_screen();

void tty_print_char(char c);

void tty_print_str(const char *msg);

#endif /* TTY_H */
