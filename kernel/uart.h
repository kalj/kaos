#ifndef UART_H
#define UART_H

int uart_init(void);

void uart_putc(unsigned char c);

void uart_puts(const char *str);

char uart_getc(void);

#endif /* UART_H */
