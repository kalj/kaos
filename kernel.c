#include "kaos_int.h"
#include "tty.h"
#include "uart.h"
#include "irq.h"

void print_hex4(uint8_t v) {
    char c = v < 10 ? '0' + v : 'a' + (v - 10);
    tty_print_char(c);
}

void print_hex8(uint8_t v) {
    print_hex4(v >> 4);
    print_hex4(v & 0xf);
}

#define PRINT_NIBBLES(v, n)                   \
    do {                                      \
        for (int i = (n - 1); i >= 0; i--) {  \
            print_hex4((v >> (i * 4)) & 0xf); \
        }                                     \
    } while (0)

void print_hex16(uint16_t v) {
    PRINT_NIBBLES(v, 4);
}

void print_hex32(uint32_t v) {
    PRINT_NIBBLES(v, 8);
}

void print_hex64(uint64_t v) {
    PRINT_NIBBLES(v, 16);
}

void hang() {
    while (1) {
        asm volatile("hlt");
    }
}

typedef struct __attribute__((__packed__)) {
    uint64_t base;
    uint64_t size;
    uint32_t type;
    uint32_t extra;
} MemoryMapEntry;

MemoryMapEntry *memory_map = (MemoryMapEntry *)0x7e00;

void print_mem_map() {
    tty_print_str("Base               Size               Type\n");
    for (int i = 0; i < 5; i++) {
        print_hex64(memory_map[i].base);
        tty_print_str("   ");
        print_hex64(memory_map[i].size);
        tty_print_str("   ");
        if (memory_map[i].type == 1) {
            tty_print_str("M      ");
        } else if (memory_map[i].type == 2) {
            tty_print_str("R      ");
        } else {
            tty_print_str("?      ");
        }
        tty_print_str("\n");
    }
}

void kmain() {
    tty_init();
    tty_print_str("\n");
    /* clear_screen(); */
    tty_print_str("Hello from kernel.c!\n");

    tty_print_str("Memory map:\n");
    print_mem_map();

    if (uart_init() == 0) {
        tty_print_str("uart initialized ok\n");
        uart_puts("uart initialized ok\n");
    }

    
    irq_init();



    hang();
}
