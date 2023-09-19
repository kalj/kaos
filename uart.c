#include "uart.h"

#include "port_io.h"

#define SERIAL_PORT 0x3f8

int uart_init() {
    outportb(SERIAL_PORT + 1, 0x00);  // Disable all interrupts
    outportb(SERIAL_PORT + 3, 0x80);  // Enable DLAB (set baud rate divisor)
    outportb(SERIAL_PORT + 0, 0x01);  // Set divisor to 1 (LSB)
    outportb(SERIAL_PORT + 1, 0x00);  //                  (MSB)
    outportb(SERIAL_PORT + 3, 0x03);  // Enable DLAB, and set 8N1

    outportb(SERIAL_PORT + 2, 0xC7);  // Enable FIFO, clear them, with 14-byte threshold
    outportb(SERIAL_PORT + 4, 0x0B);  // IRQs enabled, RTS/DSR set
    outportb(SERIAL_PORT + 4, 0x1E);  // Set in loopback mode, test the serial chip
    outportb(SERIAL_PORT + 0, 0xAE);  // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if (inportb(SERIAL_PORT + 0) != 0xAE) {
        return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outportb(SERIAL_PORT + 4, 0x0F);

    return 0;
}

void uart_putc(char c) {
    outportb(SERIAL_PORT + 0, c);
}

void uart_puts(const char *str)
{
    while(*str)
    {
        uart_putc(*str++);
    }
}
