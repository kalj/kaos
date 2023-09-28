#include "uart.h"

#include "portio.h"

#define SERIAL_PORT 0x3f8

int uart_init() {
    portio_outb(SERIAL_PORT + 1, 0x00);  // Disable all interrupts
    portio_outb(SERIAL_PORT + 3, 0x80);  // Enable DLAB (set baud rate divisor)
    portio_outb(SERIAL_PORT + 0, 0x01);  // Set divisor to 1 (LSB)
    portio_outb(SERIAL_PORT + 1, 0x00);  //                  (MSB)
    portio_outb(SERIAL_PORT + 3, 0x03);  // Enable DLAB, and set 8N1

    portio_outb(SERIAL_PORT + 2, 0xC7);  // Enable FIFO, clear them, with 14-byte threshold
    portio_outb(SERIAL_PORT + 4, 0x0B);  // IRQs enabled, RTS/DSR set
    portio_outb(SERIAL_PORT + 4, 0x1E);  // Set in loopback mode, test the serial chip
    portio_outb(SERIAL_PORT + 0, 0xAE);  // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if (portio_inb(SERIAL_PORT + 0) != 0xAE) {
        return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    portio_outb(SERIAL_PORT + 4, 0x0F);

    return 0;
}

void uart_putc(char c) {
    portio_outb(SERIAL_PORT + 0, c);
}

void uart_puts(const char *str)
{
    while(*str)
    {
        uart_putc(*str++);
    }
}
