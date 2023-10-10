#ifndef UART_H
#define UART_H

int uart_init();

void uart_putc(char c);

void uart_puts(const char *str);

char uart_getc();

#endif /* UART_H */
