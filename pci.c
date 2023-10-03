#include "pci.h"

#include "kaos.h"
#include "memcpy.h"
#include "portio.h"
#include "strfmt.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

uint32_t pci_read_reg32(uint8_t bus, uint8_t device, uint8_t func, uint8_t reg)
{
    uint32_t address = (1 << 31) | (bus << 16) | ((device & 0x1f) << 11) | ((func & 0x7) << 8) | (reg & 0xFC);

    asm volatile("outl %0, %1" : : "a"(address), "Nd"(PCI_CONFIG_ADDRESS) : "memory");

    uint32_t res;
    asm volatile("inl %1, %0" : "=a"(res) : "Nd"(PCI_CONFIG_DATA) : "memory");

    return res;
}

void pci_enumerate()
{
    kaos_puts("PCI devices:\n");
    char buf[80];
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
                uint8_t header_type                     = (pci_read_reg32(bus, device, func, 12) >> 16) & 0xff;

                uint8_t class      = (class_subclass_progif_revision >> 24) & 0xff;
                uint8_t subclass   = (class_subclass_progif_revision >> 16) & 0xff;
                uint8_t progIf     = (class_subclass_progif_revision >> 8) & 0xff;
                uint8_t revisionId = class_subclass_progif_revision & 0xff;

                int buflen = 80;
                char * bufptr = buf;
                strfmt_u8_hex(buf, 80, bus);
                bufptr+=2; buflen-=2;

                *bufptr++ = ':';
                buflen--;

                strfmt_u8_hex(bufptr, buflen, device);
                bufptr+=2; buflen-=2;

                strfmt_u8_hex(bufptr, buflen, func);
                bufptr+=2; buflen-=2;
                bufptr[-2] = '.'; // overwrite leading 0 with a .

                memcpy(bufptr, " vendor=", 8);
                bufptr += 8; buflen -= 8;

                strfmt_u16_hex(bufptr, buflen, vendorId);
                bufptr += 4; buflen -= 4;

                memcpy(bufptr, " device=", 8);
                bufptr += 8; buflen -= 8;

                strfmt_u16_hex(bufptr, buflen, deviceId);
                bufptr += 4; buflen -= 4;

                memcpy(bufptr, " header=", 8);
                bufptr += 8; buflen -= 8;

                strfmt_u8_hex(bufptr, buflen, header_type);
                bufptr += 2; buflen -= 2;

                memcpy(bufptr, " class=", 7);
                bufptr += 7; buflen -= 7;

                strfmt_u8_hex(bufptr, buflen, class);
                bufptr += 2; buflen -= 2;

                memcpy(bufptr, " subclass=", 10);
                bufptr += 10; buflen -= 10;
                strfmt_u8_hex(bufptr, buflen, subclass);
                bufptr += 2; buflen -= 2;

                memcpy(bufptr, " progIf=", 8);
                bufptr += 8; buflen -= 8;
                strfmt_u8_hex(bufptr, buflen, progIf);
                bufptr += 2; buflen -= 2;

                memcpy(bufptr, " rev=", 5);
                bufptr += 5; buflen -= 5;
                strfmt_u8_hex(bufptr, buflen, revisionId);
                bufptr += 2; buflen -= 2;

                *bufptr++ = '\n';
                *bufptr++ = '\0';

                kaos_puts(buf);
            }
        }
    }
}
