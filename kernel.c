#include "irq.h"
#include "kaos.h"
#include "kaos_int.h"
#include "keyboard.h"
#include "pci.h"
#include "pic.h"
#include "strfmt.h"
#include "tty.h"
#include "uart.h"

#include "bios_system_info.h"

void hang()
{
    while (1) {
        asm volatile("hlt");
    }
}

void print_memory_map()
{
    char buf[100];
    kaos_puts("Size of Low Memory: ");
    if(strfmt_s32_dec(buf, sizeof(buf), LOWMEM_SIZE_KB) == 0) return;
    kaos_puts(buf);
    kaos_puts(" KiB\n");

    struct MemoryMapEntry *memory_map = MEMORY_MAP_ARRAY;

    kaos_puts("Memory map:\n");
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
/*     kaos_puts("timer fired!\n"); */
/*     pic_eoi(0); */
/* } */

/* #define IRQ_VEC_TIMER 32 */

void kmain()
{
    tty_init();
    uart_init();
    kaos_setup_stdout(TRUE, TRUE);
    kaos_puts("\n");
    /* clear_screen(); */
    kaos_puts("Hello from kernel.c!\n\n");

    print_memory_map();

    kaos_puts("\n");

    pci_enumerate();

    /* pic_init(); */
    /* irq_init(); */

    /* keyboard_init(); */
    /* pic_enable_interrupt(PIC_IRQ_TIMER); */
    /* irq_register_handler(IRQ_VEC_TIMER, timer_handler, 0x8E); */

    /* irq_enable(); */

    /** shell:
     * while(true) {
     *    getline();
     *    get_first_word()
     *    if(strequal(word, "quit")) {
     *       // break while loop and end
     *    } else if(strequal(word, "echo")) {
     *      // print any other args 
     *    } else if(strequal(word, "print_memory_map")) {
     *      // run print_memory_map()
     *    } else if(strequal(word, "time")) {
     *      // print time
     *    } else {
     *       // unknown: panic
     *    }
     * }
     */

    hang();
}
