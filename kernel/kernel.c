#include "bios_system_info.h"
#include "cmos.h"
#include "floppy.h"
#include "i8254x.h"
#include "irq.h"
#include "kaos.h"
#include "kaos_int.h"
#include "keyboard.h"
#include "pci.h"
#include "pic.h"
#include "stddef.h"
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
    kaos_printf("Size of Low Memory: %d KiB\n", (int)LOWMEM_SIZE_KB);

    struct MemoryMapEntry *memory_map = MEMORY_MAP_ARRAY;

    kaos_puts("Memory map:\n");
    kaos_puts(" Base               Size               Type\n");
    for (int i = 0; i < MEMORY_MAP_NUM; i++) {
        const char *type_str = memory_map[i].type == 1 ? "M" : memory_map[i].type == 2 ? "R" : "?";
        kaos_printf(" %q   %q   %s\n", memory_map[i].base, memory_map[i].size, type_str);
    }
}

const char *floppy_type_string(uint8_t type)
{
    switch (type) {
        case 0:
            return "no drive";
        case 1:
            return "360 KB 5.25";
        case 2:
            return "1.2 MB 5.25";
        case 3:
            return "720 KB 3.5";
        case 4:
            return "1.44 MB 3.5";
        case 5:
            return "2.88 MB 3.5";
        default:
            return "invalid";
    }
}

const char *weekday_to_string(uint8_t day)
{
    switch (day) {
        case 1:
            return "Sunday";
        case 2:
            return "Monday";
        case 3:
            return "Tuesday";
        case 4:
            return "Wednesday";
        case 5:
            return "Thursday";
        case 6:
            return "Friday";
        case 7:
            return "Saturday";
        default:
            return "invalid";
    }
}

void print_cmos_reg(uint8_t reg)
{
    uint8_t value = cmos_read_reg(reg);
    kaos_printf(" CMOS Reg %b: %b\n", reg, value);
}

void print_cmos_stuff()
{
    kaos_puts("\nCMOS Stuff:\n");
    uint8_t bcd_century = cmos_read_reg(0x32);
    uint8_t bcd_year    = cmos_read_reg(0x9);
    uint8_t bcd_month   = cmos_read_reg(0x8);
    uint8_t bcd_day     = cmos_read_reg(0x7);
    uint8_t bcd_weekday = cmos_read_reg(0x6);
    uint8_t bcd_hour    = cmos_read_reg(0x4);
    uint8_t bcd_min     = cmos_read_reg(0x2);
    uint8_t bcd_sec     = cmos_read_reg(0x0);

    kaos_printf(" Time %b%b-%b-%b %s %b:%b:%b\n",
                bcd_century,
                bcd_year,
                bcd_month,
                bcd_day,
                weekday_to_string(bcd_weekday),
                bcd_hour,
                bcd_min,
                bcd_sec);

    uint8_t status_reg_a = cmos_read_reg(0x0a);
    uint8_t status_reg_b = cmos_read_reg(0x0b);
    uint8_t status_reg_c = cmos_read_reg(0x0c);
    uint8_t status_reg_d = cmos_read_reg(0x0d);
    kaos_printf(" StatusA: %b\n", status_reg_a);
    kaos_printf(" StatusB: %b\n", status_reg_b);
    kaos_printf(" StatusC: %b\n", status_reg_c);
    kaos_printf(" StatusD: %b\n", status_reg_d);

    uint8_t floppy_type = cmos_read_reg(0x10);
    kaos_printf(" Master Floppy: %s\n", floppy_type_string(0xf & (floppy_type >> 4)));
    kaos_printf(" Slave Floppy:  %s\n", floppy_type_string(0xf & floppy_type));

    uint8_t equipment = cmos_read_reg(0x14);
    kaos_puts(" Equipment:\n");
    kaos_printf("  Number of floppy drives: %d\n", ((equipment >> 6) & 0x3) + 1);
    kaos_printf("  Monitor type:            %d\n", (equipment >> 4) & 0x3);
    kaos_printf("  Display enabed:          %d\n", (equipment >> 3) & 0x1);
    kaos_printf("  Keyboard enabed:         %d\n", (equipment >> 2) & 0x1);
    kaos_printf("  Math coproc installed:   %d\n", (equipment >> 1) & 0x1);
    kaos_printf("  Floppy drive installed:  %d\n", equipment & 0x1);

    uint8_t base_mem_lb = cmos_read_reg(0x15);
    uint8_t base_mem_hb = cmos_read_reg(0x16);
    kaos_printf(" Base memory size: %d KB\n", base_mem_lb | (base_mem_hb << 8));

    uint8_t ext_mem_lb = cmos_read_reg(0x17);
    uint8_t ext_mem_hb = cmos_read_reg(0x18);
    kaos_printf(" Extended memory size: %d KB\n", ext_mem_lb | (ext_mem_hb << 8));

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

void print_bar(const struct PciEntry *entry, int barIdx)
{
    const struct PciBar *bar = &entry->bars[barIdx];
    switch (bar->type) {
        case BAR_TYPE_NONE:
            kaos_printf("  Bar%d: -\n", barIdx);
            break;
        case BAR_TYPE_IO:
            kaos_printf("  Bar%d: IO:%l size: %l\n", barIdx, bar->addr, bar->size);
            break;
        case BAR_TYPE_MEM:
            kaos_printf("  Bar%d: MEM:%l type: %b pf: %b size: %l\n",
                        barIdx,
                        bar->addr,
                        bar->prefetchable,
                        bar->mem_type,
                        bar->size);
    }
}

bool print_pci_entry_verbose = FALSE;

void handle_pci_entry(const struct PciEntry *entry)
{
    uint8_t bus    = entry->bus;
    uint8_t device = entry->device;
    uint8_t func   = entry->func;

    const char *class_descr = pci_get_class_description(entry);

    const char *device_descr = "Unknown device";

    // detect
    if (entry->vendorId == 0x8086) {
        if (entry->deviceId == 0x1237) {
            device_descr = "440FX - 82441FX PMC";
        } else if (entry->deviceId == 0x7000) {
            device_descr = "82371SB (PIIX3) ISA";
        } else if (entry->deviceId == 0x7113) {
            device_descr = "82371AB/EB/MB PIIX4 ACPI";
            /* } else if (entry->deviceId == 0x7110) { */
            /*     device_descr = "82371AB/EB/MB PIIX4 ISA"; */
        } else if (entry->deviceId == 0x100e) {
            device_descr = "82540EM Gigabit Ethernet Controller";
        }
    }

    // ---------------------------------------
    // First line
    // ---------------------------------------

    kaos_printf("%b:%b.%c %s - %s\n", entry->bus, entry->device, '0' + entry->func, class_descr, device_descr);

    if (!print_pci_entry_verbose) return;

    // Vendor, device, & revision
    kaos_printf("  Vendor,Device,Revision: %w,%w,%b\n", entry->vendorId, entry->deviceId, entry->revisionId);

    // Command & status registers
    uint32_t status_command = pci_read_reg32(entry, PCI_REG_STATUS_COMMAND);
    kaos_printf("  Command Reg: %w\n", status_command & 0xffff);
    kaos_printf("  Status Reg: %w\n", (status_command >> 16) & 0xffff);

    // Header
    kaos_printf("  Header Type: %b (multi function: %c)\n", entry->header_type, '0' + entry->mf);

    // class, subclass, & progIf
    kaos_printf("  Class,Subclass,ProgIf: %b,%b,%b\n", entry->class_code, entry->subclass, entry->progIf);

    if (entry->header_type == 0x00) {

        print_bar(entry, 0);
        print_bar(entry, 1);
        print_bar(entry, 2);
        print_bar(entry, 3);
        print_bar(entry, 4);
        print_bar(entry, 5);

        uint32_t cardbus_cis_ptr            = pci_read_reg32(entry, 0x28);
        uint32_t subsystem_id_vendor_id     = pci_read_reg32(entry, 0x2c);
        uint32_t expansion_rom_base_address = pci_read_reg32(entry, 0x30);
        uint8_t  cap_ptr                    = pci_read_reg32(entry, 0x34) & 0xff;
        uint16_t subsystem_id               = (subsystem_id_vendor_id >> 16) & 0xffff;
        uint16_t subsystem_vendor_id        = subsystem_id_vendor_id & 0xffff;

        kaos_printf("  Cardbus CIS Ptr: %l\n", cardbus_cis_ptr);
        kaos_printf("  Expansion ROM base address: %l\n", expansion_rom_base_address);
        kaos_printf("  Cap Ptr: %b\n", cap_ptr);
        kaos_printf("  Subsystem ID,vendor: %w,%w\n", subsystem_id, subsystem_vendor_id);
    }

    kaos_printf("  Interrupt Line: %b\n", entry->interrupt_line);
    kaos_printf("  Interrupt Pin:  %b\n", entry->interrupt_pin);

    /* if(entry->class == 1 && entry->subclass == 1 && entry->progIf == 0x80) */
    /* { */
    /*     // IDE controller in ISA compatibility mode */
    /*     ide_init(entry); */
    /* } */
}

void init_pci_devices()
{
    print_pci_entry_verbose = TRUE;
    kaos_puts("PCI devices:\n");
    pci_foreach(handle_pci_entry);
}

uint8_t floppy_read_buffer[512];

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

    init_pci_devices();

    /* i8254x_init(); */

    cmos_init();

    print_cmos_stuff();

    int ret = floppy_init();
    if (ret) {
        kaos_puts("Failed initializing floppy");
    }

    /* uint32_t read_offset = 0x4200; */
    /* uint32_t read_offset = 0xc200; */
    uint32_t read_offset = 0x0;

    floppy_read(floppy_read_buffer, read_offset, 512);
    for (int i = 0; i < 512; i++) {
        if (i % 16 == 0) {
            kaos_printf("\n%w:", read_offset + i);
        }

        /* kaos_printf("%c", floppy_read_buffer[i]); */
        kaos_printf(" %b", floppy_read_buffer[i]);
    }

    pic_init();
    irq_init();

    keyboard_init();

    /* pic_enable_interrupt(PIC_IRQ_TIMER); */
    /* irq_register_handler(IRQ_VEC_TIMER, timer_handler, 0x8E); */

    irq_enable();

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
