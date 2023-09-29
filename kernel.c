#include "irq.h"
#include "kaos.h"
#include "kaos_int.h"
#include "keyboard.h"
#include "pic.h"
#include "strfmt.h"
#include "tty.h"
#include "uart.h"

#include "memory_map.h"

void hang() {
    while (1) {
        asm volatile("hlt");
    }
}

void print_memory_map() {

    MemoryMapEntry *memory_map = MEMORY_MAP_ARRAY;

    char buf[100];

    kaos_puts("Base               Size               Type\n");
    for (int i = 0; i < MEMORY_MAP_NUM; i++) {
        if (strfmt_u64_hex(buf, sizeof(buf), memory_map[i].base) == 0)
            return;
        kaos_puts(buf);
        kaos_puts("   ");
        if (strfmt_u64_hex(buf, sizeof(buf), memory_map[i].size) == 0)
            return;
        kaos_puts(buf);
        kaos_puts("   ");
        if (memory_map[i].type == 1) {
            kaos_puts("M      ");
        } else if (memory_map[i].type == 2) {
            kaos_puts("R      ");
        } else {
            kaos_puts("?      ");
        }
        kaos_puts("\n");
    }
}

/* static __attribute__((interrupt)) void div_error_handler(void *irq_frame) { */
/*     uart_puts("Division by 0!\n"); */
/*     asm("hlt"); */
/* } */

/* static __attribute__((interrupt)) void timer_handler(void *irq_frame) { */
/*     portio_outb(PIC1_COMMAND, PIC_EOI); */
/* } */

void kmain() {
    tty_init();
    uart_init();
    kaos_setup_stdout(TRUE, TRUE);
    kaos_puts("\n");
    /* clear_screen(); */
    kaos_puts("Hello from kernel.c!\n");

    kaos_puts("Memory map:\n");
    print_memory_map();

    pic_init();
    irq_init();

    keyboard_init();

    irq_enable();

    hang();
}
