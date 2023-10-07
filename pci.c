#include "pci.h"

#include "kaos.h"
#include "memcpy.h"
#include "portio.h"
#include "strfmt.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

static const char *class_description(uint8_t class, uint8_t subclass)
{
    switch (class) {
        case 0x0: // Unclassified
            /* switch(subclass) */
            /* { */
            /* case 0x0: return "Non-VGA-Compatible Unclassified Device"; */
            /* case 0x01: return "VGA-Compatible Unclassified Device"; */
            /* } */
            /* break; */
            return "Unclassified";
        case 0x1: // Mass Storage
            /* switch(subclass) */
            /* { */
            /* case 0x0: return "SCSI Bus Controller"; */
            /* case 0x1: return "IDE Controller"; */
            /* case 0x2: return "Floppy Disk Controller"; */
            /* case 0x3: return "IPI Bus Controller"; */
            /* case 0x4: return "RAID Controller"; */
            /* case 0x5: return "ATA Controller"; */
            /* case 0x6: return "Serial ATA Controller"; */
            /* case 0x7: return "Serial Attached SCSI Controller"; */
            /* case 0x8: return "Non-Volatile Memory Controller"; */
            /* } */
            /* break; */
            return "Mass Storage";
        case 0x2: // Network
            /* switch(subclass) */
            /* { */
            /* case 0x0: return "Ethernet Controller"; */
            /* case 0x1: return "Token Ring Controller"; */
            /* case 0x2: return "FDDI Controller"; */
            /* case 0x3: return "ATM Controller"; */
            /* case 0x4: return "ISDN Controller"; */
            /* case 0x5: return "WorldFip Controller"; */
            /* case 0x6: return "PICMG 2.14 Multi Computing Controller"; */
            /* case 0x7: return "Infiniband Controller"; */
            /* case 0x8: return "Fabric Controller"; */
            /* } */
            /* break; */
            return "Network";
        case 0x3: // Display
            /* switch(subclass) */
            /* { */
            /* case 0x0: return "VGA Compatible Controller"; */
            /* case 0x1: return "XGA Controller"; */
            /* case 0x2: return "3D Controller (Not VGA-Compatible)"; */
            /* } */
            /* break; */
            return "Display";
        case 0x4: // Multimedia
            /* switch(subclass) */
            /* { */
            /* case 0x0: return "Multimedia Video Controller"; */
            /* case 0x1: return "Multimedia Audio Controller"; */
            /* case 0x2: return "Computer Telephony Device"; */
            /* case 0x3: return "Audio Device"; */
            /* } */
            /* break; */
            return "Multimedia";
        case 0x5: // Memory
            /* switch(subclass) */
            /* { */
            /* case 0x0: return "RAM Controller"; */
            /* case 0x1: return "Flash Controller"; */
            /* } */
            /* break; */
            return "Memory";
        case 0x6: // Bridge
            /* switch(subclass) */
            /* { */
            /* case 0x0: return "Host Bridge"; */
            /* case 0x1: return "ISA Bridge"; */
            /* case 0x2: return "EISA Bridge"; */
            /* case 0x3: return "MCA Bridge"; */
            /* case 0x4: return "PCI-to-PCI Bridge"; */
            /* case 0x5: return "PCMCIA Bridge"; */
            /* case 0x6: return "NuBus Bridge"; */
            /* case 0x7: return "CardBus Bridge"; */
            /* case 0x8: return "RACEway Bridge"; */
            /* case 0x9: return "PCI-to-PCI Bridge"; */
            /* case 0x0A: return "InfiniBand-to-PCI Host Bridge"; */
            /* default: return "Unknown Bridge Controller"; */
            /* } */
            /* break; */
            return "Bridge";
        case 0x7: // Simple Communication
            return "Simple Comm";
            /* switch(subclass) */
            /* { */
            /* case 0x0: return "Serial Controller"; */
            /* case 0x1: return "Parallel Controller"; */
            /* case 0x2: return "Multiport Serial Controller"; */
            /* case 0x3: return "Modem"; */
            /* case 0x4: return "IEEE 488.1/2 (GPIB) Controller"; */
            /* case 0x5: return "Smart Card Controller "; */
            /* } */
            /* break; */
        case 0x8: // Base System Peripheral
            return "Base System Periph";
            /* switch(subclass) */
            /* { */
            /* case 0x0: return "PIC"; */
            /* case 0x1: return "DMA Controller"; */
            /* case 0x2: return "Timer"; */
            /* case 0x3: return "RTC Controller"; */
            /* case 0x4: return "PCI Hot-Plug Controller"; */
            /* case 0x5: return "SD Host Controller"; */
            /* case 0x6: return "IOMMU"; */
            /* } */
            /* break; */
        case 0x9: // Input Device
            /* switch(subclass) */
            /* { */
            /* case 0x0: return "Keyboard Controller"; */
            /* case 0x1: return "Digitizer Pen"; */
            /* case 0x2: return "Mouse Controller"; */
            /* case 0x3: return "Scanner Controller"; */
            /* case 0x4: return "Gameport Controller"; */
            /* } */
            /* break; */
            return "Input Device";
        case 0xB: // Processor
                  /*     switch(subclass) */
                  /*     { */
                  /*     case 0x0: return"386"; */
                  /*     case 0x1: return"486"; */
                  /*     case 0x2: return"Pentium"; */
                  /*     case 0x3: return"Pentium Pro"; */
                  /*     case 0x10: return"Alpha"; */
                  /*     case 0x20: return"PowerPC"; */
                  /*     case 0x30: return"MIPS"; */
                  /*     case 0x40: return"Co-Processor"; */
                  /*     } */
                  /*     break; */
            return "Processor";
        case 0xC: // Serial Bus
                  /*     switch(subclass) */
                  /*     { */
                  /*     case 0x0: return "FireWire (IEEE 1394) Controller"; 	 */
                  /*     case 0x1: return "ACCESS Bus Controller"; 	 */
                  /*     case 0x2: return "SSA"; */
                  /*     case 0x3: return "USB Controller"; */
                  /*     case 0x4: return "Fibre Channel"; */
                  /*     case 0x5: return "SMBus Controller"; */
                  /*     case 0x6: return "InfiniBand Controller"; */
                  /*     case 0x7: return "IPMI Interface"; */
                  /*     case 0x8: return "SERCOS Interface (IEC 61491)"; */
                  /*     case 0x9: return "CANbus Controller"; */
                  /*     } */
                  /*     break; */
            return "Serial Bus";
    }
    return "Unknown";
}

uint32_t pci_read_reg32(uint8_t bus, uint8_t device, uint8_t func, uint8_t reg)
{
    uint32_t address = (1 << 31) | (bus << 16) | ((device & 0x1f) << 11) | ((func & 0x7) << 8) | (reg & 0xFC);

    asm volatile("outl %0, %1"
                 :
                 : "a"(address), "Nd"(PCI_CONFIG_ADDRESS)
                 : "memory");

    uint32_t res;
    asm volatile("inl %1, %0"
                 : "=a"(res)
                 : "Nd"(PCI_CONFIG_DATA)
                 : "memory");

    return res;
}

int strlen(const char *str)
{
    int len = 0;
    while (*str++ != '\0') {
        len++;
    }
    return len;
}

void append_str(char **bufptr, int *buflen, const char *str)
{
    int len = strlen(str);
    if (len > *buflen)
        return;
    memcpy(*bufptr, str, len);
    *bufptr += len;
    *buflen -= len;
}

void append_u8_hex(char **bufptr, int *buflen, uint8_t v)
{
    strfmt_u8_hex(*bufptr, *buflen, v);
    *bufptr += 2;
    *buflen -= 2;
}

void append_u16_hex(char **bufptr, int *buflen, uint16_t v)
{
    strfmt_u16_hex(*bufptr, *buflen, v);
    *bufptr += 4;
    *buflen -= 4;
}

void append_u32_hex(char **bufptr, int *buflen, uint32_t v)
{
    strfmt_u32_hex(*bufptr, *buflen, v);
    *bufptr += 8;
    *buflen -= 8;
}

void print_bar(const char *name, uint32_t bar)
{
    char buf[80];
    char *bufptr = buf;
    int buflen   = sizeof(buf);

    append_str(&bufptr, &buflen, "  ");
    append_str(&bufptr, &buflen, name);
    append_str(&bufptr, &buflen, ": ");
    int is_io = bar & 0x1;
    if (is_io) {
        append_str(&bufptr, &buflen, " IO:");
        append_u32_hex(&bufptr, &buflen, bar & 0xfffffffc);
    } else {
        append_str(&bufptr, &buflen, "Mem:");
        append_u32_hex(&bufptr, &buflen, bar & 0xfffffff0);
        append_str(&bufptr, &buflen, " type: ");
        append_u8_hex(&bufptr, &buflen, (bar >> 1) & 0x3);
        append_str(&bufptr, &buflen, " prefetchable: ");
        append_u8_hex(&bufptr, &buflen, (bar >> 3) & 0x1);
    }

    *bufptr++ = '\n';
    *bufptr++ = '\0';
    kaos_puts(buf);
}

void pci_enumerate()
{
    kaos_puts("PCI devices:\n");
    char buf[120];
    for (int bus = 0; bus < 256; bus++) {
        for (int device = 0; device < 32; device++) {
            for (int func = 0; func < 8; func++) {
                uint32_t device_vendor = pci_read_reg32(bus, device, func, 0);

                if (device_vendor == 0xffffffff) {
                    continue;
                }

                uint16_t vendorId = device_vendor & 0xffff;
                uint16_t deviceId = (device_vendor >> 16) & 0xffff;

                /* uint32_t status_command = pci_read_reg32(bus, device, func, 4); */
                uint32_t class_subclass_progif_revision = pci_read_reg32(bus, device, func, 8);
                uint8_t header_type_byte                = (pci_read_reg32(bus, device, func, 12) >> 16) & 0xff;
                uint8_t header_type                     = header_type_byte & 0x7f;
                int mf                                  = (header_type_byte >> 7) & 1;

                uint8_t class      = (class_subclass_progif_revision >> 24) & 0xff;
                uint8_t subclass   = (class_subclass_progif_revision >> 16) & 0xff;
                uint8_t progIf     = (class_subclass_progif_revision >> 8) & 0xff;
                uint8_t revisionId = class_subclass_progif_revision & 0xff;

                const char *class_descr = class_description(class, subclass);

                const char *device_descr = 0;

                // detect
                if (vendorId == 0x8086) {
                    if (deviceId == 0x7000) {
                        device_descr = "82371SB (PIIX3) ISA";
                    } else if (deviceId == 0x100e) {
                        device_descr = "82540EM Gigabit Ethernet Controller";
                    } else if (deviceId == 0x7113) {
                        device_descr = "82371AB/EB/MB PIIX4 ACPI";
                    } else if (deviceId == 0x7110) {
                        device_descr = "82371AB/EB/MB PIIX4 ISA";
                    } else if (deviceId == 0x1237) {
                        device_descr = "440FX - 82441FX PMC";
                    }
                }

                // ---------------------------------------
                // First line
                // ---------------------------------------

                int buflen   = sizeof(buf);
                char *bufptr = buf;
                append_u8_hex(&bufptr, &buflen, bus);

                append_str(&bufptr, &buflen, ":");

                append_u8_hex(&bufptr, &buflen, device);

                append_u8_hex(&bufptr, &buflen, func);
                bufptr[-2] = '.'; // overwrite leading 0 with a .

                append_str(&bufptr, &buflen, " ");
                append_str(&bufptr, &buflen, class_descr);
                if (device_descr) {
                    append_str(&bufptr, &buflen, " - ");
                    append_str(&bufptr, &buflen, device_descr);
                }

                *bufptr++ = '\n';
                *bufptr++ = '\0';
                kaos_puts(buf);

                // Vendor, device, & revision
                bufptr = buf;
                buflen = sizeof(buf);
                append_str(&bufptr, &buflen, "  vid=");
                append_u16_hex(&bufptr, &buflen, vendorId);

                append_str(&bufptr, &buflen, " did=");
                append_u16_hex(&bufptr, &buflen, deviceId);

                append_str(&bufptr, &buflen, " rev=");
                append_u8_hex(&bufptr, &buflen, revisionId);

                /* *bufptr++ = '\n'; *bufptr++ = '\0'; kaos_puts(buf); */

                // Header
                /* bufptr = buf; buflen = sizeof(buf); */
                append_str(&bufptr, &buflen, " mf=");
                *bufptr++ = '0' + mf;
                buflen--;

                append_str(&bufptr, &buflen, " hdr=");
                append_u8_hex(&bufptr, &buflen, header_type);

                /* *bufptr++ = '\n'; *bufptr++ = '\0'; kaos_puts(buf); */

                // class, subclass, & progIf
                /* bufptr = buf; buflen = sizeof(buf); */
                append_str(&bufptr, &buflen, " cl=");
                append_u8_hex(&bufptr, &buflen, class);

                append_str(&bufptr, &buflen, " subcl=");
                append_u8_hex(&bufptr, &buflen, subclass);

                append_str(&bufptr, &buflen, " pIf=");
                append_u8_hex(&bufptr, &buflen, progIf);

                *bufptr++ = '\n';
                *bufptr++ = '\0';
                kaos_puts(buf);

                /* if(header_type == 0x00) */
                /* { */

                /*     uint32_t bar0 = pci_read_reg32(bus, device, func, 0x10); */
                /*     uint32_t bar1 = pci_read_reg32(bus, device, func, 0x14); */
                /*     uint32_t bar2 = pci_read_reg32(bus, device, func, 0x18); */
                /*     uint32_t bar3 = pci_read_reg32(bus, device, func, 0x20); */
                /*     uint32_t bar4 = pci_read_reg32(bus, device, func, 0x24); */
                /*     uint32_t bar5 = pci_read_reg32(bus, device, func, 0x28); */
                /*     uint32_t subsystem_id_vendor_id = pci_read_reg32(bus, device, func, 0x2c); */
                /*     uint32_t expansion_rom_base_address = pci_read_reg32(bus, device, func, 0x30); */
                /*     uint16_t subsystem_id = (subsystem_id_vendor_id>>16)&0xffff; */
                /*     uint16_t subsystem_vendor_id = subsystem_id_vendor_id&0xffff; */

                /*     print_bar("bar0", bar0); */
                /*     print_bar("bar1", bar1); */
                /*     print_bar("bar2", bar2); */
                /*     print_bar("bar3", bar3); */
                /*     print_bar("bar4", bar4); */
                /*     print_bar("bar5", bar5); */

                /*     bufptr = buf; buflen = sizeof(buf); */
                /*     append_str(&bufptr, &buflen, "  Subsystem ID="); */
                /*     append_u16_hex(&bufptr, &buflen, subsystem_id); */
                /*     append_str(&bufptr, &buflen, " vendor="); */
                /*     append_u16_hex(&bufptr, &buflen, subsystem_vendor_id); */
                /*     *bufptr++ = '\n'; *bufptr++ = '\0'; kaos_puts(buf); */

                /*     bufptr = buf; buflen = sizeof(buf); */
                /*     append_str(&bufptr, &buflen, "  Expansion ROM base address="); */
                /*     append_u32_hex(&bufptr, &buflen, expansion_rom_base_address); */
                /*     *bufptr++ = '\n'; *bufptr++ = '\0'; kaos_puts(buf); */
                /* } */
            }
        }
    }
}
