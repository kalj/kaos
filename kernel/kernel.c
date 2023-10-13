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
    strfmt_snprintf(buf, 100, "Size of Low Memory: %d KiB\n", (int)LOWMEM_SIZE_KB);
    kaos_puts(buf);

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
    strfmt_snprintf(buf, sizeof(buf), " CMOS Reg %b: %b\n", reg, value);
    kaos_puts(buf);
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

    strfmt_snprintf(buf,
                    sizeof(buf),
                    " Time %b%b-%b-%b %s %b:%b:%b\n",
                    bcd_century,
                    bcd_year,
                    bcd_month,
                    bcd_day,
                    weekday_to_string(bcd_weekday),
                    bcd_hour,
                    bcd_min,
                    bcd_sec);
    kaos_puts(buf);

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

void print_bar(const struct PciEntry *entry, int barIdx)
{
    uint8_t bar_address = 0x10 + 4 * barIdx;
    uint32_t bar        = pci_read_reg32(entry->bus, entry->device, entry->func, bar_address);

    char buf[80];
    char *bufptr = buf;
    int buflen   = sizeof(buf);

    append_str(&bufptr, &buflen, "  Bar");
    append_s32_dec(&bufptr, &buflen, barIdx);
    append_str(&bufptr, &buflen, ": ");

    int is_io          = bar & 0x1;
    uint32_t addr_mask = 0xfffffff0;
    if (is_io) {
        addr_mask = 0xfffffffc;
    }

    pci_write_reg32(entry->bus, entry->device, entry->func, bar_address, 0xffffffff);
    uint32_t bar_size = pci_read_reg32(entry->bus, entry->device, entry->func, bar_address);
    bar_size          = ~(addr_mask & bar_size) + 1;
    // restore original value
    pci_write_reg32(entry->bus, entry->device, entry->func, bar_address, bar);

    if (bar_size == 0) {
        append_str(&bufptr, &buflen, " -");
    } else {
        if (is_io) {
            append_str(&bufptr, &buflen, " IO:");
            append_u32_hex(&bufptr, &buflen, bar & 0xfffffffc);
        } else {
            append_str(&bufptr, &buflen, "Mem:");
            append_u32_hex(&bufptr, &buflen, bar & 0xfffffff0);
            append_str(&bufptr, &buflen, " type: ");
            append_u8_hex(&bufptr, &buflen, (bar >> 1) & 0x3);
            append_str(&bufptr, &buflen, " pf: ");
            append_u8_hex(&bufptr, &buflen, (bar >> 3) & 0x1);
        }

        append_str(&bufptr, &buflen, " size: ");
        append_u32_hex(&bufptr, &buflen, bar_size);
    }

    *bufptr++ = '\n';
    *bufptr++ = '\0';
    kaos_puts(buf);
}

bool print_pci_entry_verbose = FALSE;

void handle_pci_entry(const struct PciEntry *entry)
{
    uint8_t bus    = entry->bus;
    uint8_t device = entry->device;
    uint8_t func   = entry->func;

    char buf[120];

    const char *class_descr = pci_get_class_description(entry);

    const char *device_descr = 0;

    // detect
    if (entry->vendorId == 0x8086) {
        if (entry->deviceId == 0x7000) {
            device_descr = "82371SB (PIIX3) ISA";
        } else if (entry->deviceId == 0x100e) {
            device_descr = "82540EM Gigabit Ethernet Controller";
        } else if (entry->deviceId == 0x7113) {
            device_descr = "82371AB/EB/MB PIIX4 ACPI";
        } else if (entry->deviceId == 0x7110) {
            device_descr = "82371AB/EB/MB PIIX4 ISA";
        } else if (entry->deviceId == 0x1237) {
            device_descr = "440FX - 82441FX PMC";
        }
    }

    // ---------------------------------------
    // First line
    // ---------------------------------------

    int buflen   = sizeof(buf);
    char *bufptr = buf;
    append_u8_hex(&bufptr, &buflen, entry->bus);

    append_str(&bufptr, &buflen, ":");

    append_u8_hex(&bufptr, &buflen, entry->device);

    append_u8_hex(&bufptr, &buflen, entry->func);
    bufptr[-2] = '.'; // overwrite leading 0 with a .

    append_str(&bufptr, &buflen, " ");
    append_str(&bufptr, &buflen, class_descr);
    if (device_descr) {
        append_str(&bufptr, &buflen, " - ");
        append_str(&bufptr, &buflen, device_descr);
    } else {
        append_str(&bufptr, &buflen, "  vid=");
        append_u16_hex(&bufptr, &buflen, entry->vendorId);

        append_str(&bufptr, &buflen, " did=");
        append_u16_hex(&bufptr, &buflen, entry->deviceId);

        append_str(&bufptr, &buflen, " rev=");
        append_u8_hex(&bufptr, &buflen, entry->revisionId);
    }

    *bufptr++ = '\n';
    *bufptr++ = '\0';
    kaos_puts(buf);

    if (!print_pci_entry_verbose)
        return;

    // Vendor, device, & revision
    strfmt_snprintf(buf,
                    sizeof(buf),
                    "  Vendor,Device,Revision: %w,%w,%b\n",
                    entry->vendorId,
                    entry->deviceId,
                    entry->revisionId);
    kaos_puts(buf);

    // Command & status registers
    bufptr = buf;
    buflen = sizeof(buf);
    append_str(&bufptr, &buflen, "  Command Reg: ");
    append_u16_hex(&bufptr, &buflen, entry->command);
    *bufptr++ = '\n';
    *bufptr++ = '\0';
    kaos_puts(buf);

    bufptr = buf;
    buflen = sizeof(buf);
    append_str(&bufptr, &buflen, "  Status Reg: ");
    append_u16_hex(&bufptr, &buflen, entry->status);
    *bufptr++ = '\n';
    *bufptr++ = '\0';
    kaos_puts(buf);

    // Header
    bufptr = buf;
    buflen = sizeof(buf);
    append_str(&bufptr, &buflen, "  Header Type: ");
    append_u8_hex(&bufptr, &buflen, entry->header_type);
    append_str(&bufptr, &buflen, " (multi function: ");
    *bufptr++ = '0' + entry->mf;
    buflen--;
    append_str(&bufptr, &buflen, ")");
    *bufptr++ = '\n';
    *bufptr++ = '\0';
    kaos_puts(buf);

    // class, subclass, & progIf
    strfmt_snprintf(buf,
                    sizeof(buf),
                    "  Class,Subclass,ProgIf: %b,%b,%b\n",
                    entry->class,
                    entry->subclass,
                    entry->progIf);
    kaos_puts(buf);

    if (entry->header_type == 0x00) {

        print_bar(entry, 0);
        print_bar(entry, 1);
        print_bar(entry, 2);
        print_bar(entry, 3);
        print_bar(entry, 4);
        print_bar(entry, 5);

        uint32_t cardbus_cis_ptr            = pci_read_reg32(bus, device, func, 0x28);
        uint32_t subsystem_id_vendor_id     = pci_read_reg32(bus, device, func, 0x2c);
        uint32_t expansion_rom_base_address = pci_read_reg32(bus, device, func, 0x30);
        uint8_t cap_ptr                     = pci_read_reg32(bus, device, func, 0x34) & 0xff;
        uint16_t subsystem_id               = (subsystem_id_vendor_id >> 16) & 0xffff;
        uint16_t subsystem_vendor_id        = subsystem_id_vendor_id & 0xffff;

        bufptr = buf;
        buflen = sizeof(buf);
        append_str(&bufptr, &buflen, "  Cardbus CIS Ptr: ");
        append_u32_hex(&bufptr, &buflen, cardbus_cis_ptr);
        *bufptr++ = '\n';
        *bufptr++ = '\0';
        kaos_puts(buf);

        bufptr = buf;
        buflen = sizeof(buf);
        append_str(&bufptr, &buflen, "  Expansion ROM base address: ");
        append_u32_hex(&bufptr, &buflen, expansion_rom_base_address);
        *bufptr++ = '\n';
        *bufptr++ = '\0';
        kaos_puts(buf);

        bufptr = buf;
        buflen = sizeof(buf);
        append_str(&bufptr, &buflen, "  Cap Ptr:");
        append_u8_hex(&bufptr, &buflen, cap_ptr);
        *bufptr++ = '\n';
        *bufptr++ = '\0';
        kaos_puts(buf);

        bufptr = buf;
        buflen = sizeof(buf);
        append_str(&bufptr, &buflen, "  Subsystem ID,vendor: ");
        append_u16_hex(&bufptr, &buflen, subsystem_id);
        append_str(&bufptr, &buflen, ",");
        append_u16_hex(&bufptr, &buflen, subsystem_vendor_id);
        *bufptr++ = '\n';
        *bufptr++ = '\0';
        kaos_puts(buf);
    }

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

    floppy_init();

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
