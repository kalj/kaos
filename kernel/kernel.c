#include "bios_system_info.h"
#include "cmos.h"
#include "i8254x.h"
#include "irq.h"
#include "kaos.h"
#include "kaos_int.h"
#include "keyboard.h"
#include "pci.h"
#include "pic.h"
#include "strfmt.h"
#include "tty.h"
#include "uart.h"

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
    if (strfmt_s32_dec(buf, sizeof(buf), LOWMEM_SIZE_KB) == 0)
        return;
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

const char *floppy_type_string(uint8_t type)
{
    switch (type) {
        case 0: return "no drive";
        case 1: return "360 KB 5.25";
        case 2: return "1.2 MB 5.25";
        case 3: return "720 KB 3.5";
        case 4: return "1.44 MB 3.5";
        case 5: return "2.88 MB 3.5";
        default: return "invalid";
    }
}

const char *weekday_to_string(uint8_t day)
{
    switch (day) {
        case 1: return "Sunday";
        case 2: return "Monday";
        case 3: return "Tuesday";
        case 4: return "Wednesday";
        case 5: return "Thursday";
        case 6: return "Friday";
        case 7: return "Saturday";
        default: return "invalid";
    }
}

void print_cmos_reg(uint8_t reg)
{
    uint8_t value = cmos_read_reg(reg);
    char buf[80];
    kaos_puts(" CMOS Reg ");
    strfmt_u8_hex(buf, 80, reg); kaos_puts(buf);
    kaos_puts(": ");
    strfmt_u8_hex(buf, 80, value); 
    kaos_puts(buf); kaos_puts("\n");
}

void print_cmos_stuff()
{
    kaos_puts("\nCMOS Stuff:\n");
    char buf[80];
    uint8_t bcd_century = cmos_read_reg(0x32);
    uint8_t bcd_year    = cmos_read_reg(0x9);
    uint8_t bcd_month   = cmos_read_reg(0x8);
    uint8_t bcd_day     = cmos_read_reg(0x7);
    uint8_t bcd_weekday = cmos_read_reg(0x6);
    uint8_t bcd_hour    = cmos_read_reg(0x4);
    uint8_t bcd_min     = cmos_read_reg(0x2);
    uint8_t bcd_sec     = cmos_read_reg(0x0);
    kaos_puts(" Time: ");
    strfmt_u8_hex(buf, 80, bcd_century);
    kaos_puts(buf);
    strfmt_u8_hex(buf, 80, bcd_year);
    kaos_puts(buf);
    kaos_puts("-");
    strfmt_u8_hex(buf, 80, bcd_month);
    kaos_puts(buf);
    kaos_puts("-");
    strfmt_u8_hex(buf, 80, bcd_day);
    kaos_puts(buf);
    kaos_puts(" ");

    kaos_puts(weekday_to_string(bcd_weekday));
    kaos_puts(" ");

    strfmt_u8_hex(buf, 80, bcd_hour);
    kaos_puts(buf);
    kaos_puts(":");
    strfmt_u8_hex(buf, 80, bcd_min);
    kaos_puts(buf);
    kaos_puts(":");
    strfmt_u8_hex(buf, 80, bcd_sec);
    kaos_puts(buf);
    kaos_puts("\n");

    uint8_t status_reg_a = cmos_read_reg(0x0a);
    uint8_t status_reg_b = cmos_read_reg(0x0b);
    uint8_t status_reg_c = cmos_read_reg(0x0c);
    uint8_t status_reg_d = cmos_read_reg(0x0d);
    kaos_puts(" StatusA: "); strfmt_u8_hex(buf, 80, status_reg_a); kaos_puts(buf); kaos_puts("\n");
    kaos_puts(" StatusB: "); strfmt_u8_hex(buf, 80, status_reg_b); kaos_puts(buf); kaos_puts("\n");
    kaos_puts(" StatusC: "); strfmt_u8_hex(buf, 80, status_reg_c); kaos_puts(buf); kaos_puts("\n");
    kaos_puts(" StatusD: "); strfmt_u8_hex(buf, 80, status_reg_d); kaos_puts(buf); kaos_puts("\n");

    uint8_t floppy_type = cmos_read_reg(0x10);
    kaos_puts(" Master Floppy: ");
    kaos_puts(floppy_type_string(0xf & (floppy_type >> 4)));
    kaos_puts("\n");
    kaos_puts(" Slave Floppy:  ");
    kaos_puts(floppy_type_string(0xf & (floppy_type)));
    kaos_puts("\n");
    uint8_t equipment = cmos_read_reg(0x14);
    kaos_puts(" Equipment:\n");
    kaos_puts("  Number of floppy drives: ");
    strfmt_s32_dec(buf,80,1+((equipment>>6)&0x3));
    kaos_puts(buf);
    kaos_puts("\n");
    kaos_puts("  Monitor type:            "); strfmt_s32_dec(buf,80,(equipment>>4)&0x3); kaos_puts(buf); kaos_puts("\n");
    kaos_puts("  Display enabled:         "); strfmt_s32_dec(buf,80,(equipment>>3)&0x1); kaos_puts(buf); kaos_puts("\n");
    kaos_puts("  Keyboard enabled:        "); strfmt_s32_dec(buf,80,(equipment>>2)&0x1); kaos_puts(buf); kaos_puts("\n");
    kaos_puts("  Math coproc installed:   "); strfmt_s32_dec(buf,80,(equipment>>1)&0x1); kaos_puts(buf); kaos_puts("\n");
    kaos_puts("  Floppy drive installed:  "); strfmt_s32_dec(buf,80,equipment&0x1); kaos_puts(buf); kaos_puts("\n");

    uint8_t base_mem_lb = cmos_read_reg(0x15);
    uint8_t base_mem_hb = cmos_read_reg(0x16);
    kaos_puts(" Base memory size: "); strfmt_s32_dec(buf, 80, base_mem_lb|(base_mem_hb<<8)); kaos_puts(buf); kaos_puts(" KB\n");

    uint8_t ext_mem_lb = cmos_read_reg(0x17);
    uint8_t ext_mem_hb = cmos_read_reg(0x18);
    kaos_puts(" Extended memory size: "); strfmt_s32_dec(buf, 80, ext_mem_lb|(ext_mem_hb<<8)); kaos_puts(buf); kaos_puts(" KB\n");

#if 0
    print_cmos_reg(1);
    print_cmos_reg(3);
    print_cmos_reg(5);
    print_cmos_reg(0xe);
    print_cmos_reg(0xf);
    print_cmos_reg(0x11);
    print_cmos_reg(0x12);
    print_cmos_reg(0x13);
    for(int i=0x19; i<0x80; i++) {
        print_cmos_reg(i);
    }
#endif
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

    /* i8254x_init(); */

    cmos_init();

    print_cmos_stuff();

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
