#include "uart.h"

#include "portio.h"

#define SERIAL_PORT 0x3f8

#define IRQ_ID_FIFO_CONTROL_REGISTER (SERIAL_PORT + 2)
#define LINE_CONTROL_REGISTER        (SERIAL_PORT + 3)
#define MODEM_CONTROL_REGISTER       (SERIAL_PORT + 4)
#define LINE_STATUS_REGISTER         (SERIAL_PORT + 5)

#define TRANSMITTER_EMPTY 0x20

int uart_init()
{
    portio_outb(SERIAL_PORT + 1, 0x00);       // Disable all interrupts
    portio_outb(LINE_CONTROL_REGISTER, 0x80); // Enable DLAB (set baud rate divisor)
    portio_outb(SERIAL_PORT + 0, 0x01);       // Set divisor to 1 (LSB)
    portio_outb(SERIAL_PORT + 1, 0x00);       //                  (MSB)
    portio_outb(LINE_CONTROL_REGISTER, 0x03); // Enable DLAB, and set 8N1

    portio_outb(IRQ_ID_FIFO_CONTROL_REGISTER, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    portio_outb(MODEM_CONTROL_REGISTER, 0x0B);       // IRQs enabled, RTS/DSR set
    portio_outb(MODEM_CONTROL_REGISTER, 0x1E);       // Set in loopback mode, test the serial chip
    uart_putc(0xAE); // Test serial chip (send byte 0xAE and check if serial returns same byte) */

    // Check if serial is faulty (i.e: not same byte as sent)
    if ((unsigned char)uart_getc() != 0xAE) {
        return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    portio_outb(MODEM_CONTROL_REGISTER, 0x0F);

    return 0;
}

void uart_putc(unsigned char c)
{
    uint8_t status = portio_inb(LINE_STATUS_REGISTER);
    while ((status & TRANSMITTER_EMPTY) == 0) {
        status = portio_inb(LINE_STATUS_REGISTER);
    }

    portio_outb(SERIAL_PORT + 0, c);
}

void uart_puts(const char *str)
{
    while (*str) {
        if (*str == '\n') {
            uart_putc('\r');
        }
        uart_putc(*str++);
    }
}

char uart_getc()
{
    while ((portio_inb(LINE_STATUS_REGISTER) & 1) == 0) {
    }

    return portio_inb(SERIAL_PORT + 0);
}
