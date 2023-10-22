#include "pci.h"

#include "portio.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

static struct PciEntry current_entry;

const char *pci_get_class_description(const struct PciEntry *entry)
{
    uint8_t class    = entry->class_code;
    uint8_t subclass = entry->subclass;
    /* uint8_t progIf = entry->progIf; */

    switch (class) {
        case 0x0: // Unclassified
            switch (subclass) {
                case 0x0:
                    return "Non-VGA-Compatible Unclassified Device";
                case 0x01:
                    return "VGA-Compatible Unclassified Device";
            }
            break;
            /* return "Unclassified"; */
        case 0x1: // Mass Storage
            switch (subclass) {
                case 0x0:
                    return "SCSI Bus Controller";
                case 0x1:
                    return "IDE Controller";
                case 0x2:
                    return "Floppy Disk Controller";
                case 0x3:
                    return "IPI Bus Controller";
                case 0x4:
                    return "RAID Controller";
                case 0x5:
                    return "ATA Controller";
                case 0x6:
                    return "Serial ATA Controller";
                case 0x7:
                    return "Serial Attached SCSI Controller";
                case 0x8:
                    return "Non-Volatile Memory Controller";
            }
            break;
            /* return "Mass Storage"; */
        case 0x2: // Network
            switch (subclass) {
                case 0x0:
                    return "Ethernet Controller";
                case 0x1:
                    return "Token Ring Controller";
                case 0x2:
                    return "FDDI Controller";
                case 0x3:
                    return "ATM Controller";
                case 0x4:
                    return "ISDN Controller";
                case 0x5:
                    return "WorldFip Controller";
                case 0x6:
                    return "PICMG 2.14 Multi Computing Controller";
                case 0x7:
                    return "Infiniband Controller";
                case 0x8:
                    return "Fabric Controller";
            }
            break;
            /* return "Network"; */
        case 0x3: // Display
            switch (subclass) {
                case 0x0:
                    return "VGA Compatible Controller";
                case 0x1:
                    return "XGA Controller";
                case 0x2:
                    return "3D Controller (Not VGA-Compatible)";
            }
            break;
            /* return "Display"; */
        case 0x4: // Multimedia
            switch (subclass) {
                case 0x0:
                    return "Multimedia Video Controller";
                case 0x1:
                    return "Multimedia Audio Controller";
                case 0x2:
                    return "Computer Telephony Device";
                case 0x3:
                    return "Audio Device";
            }
            break;
            /* return "Multimedia"; */
        case 0x5: // Memory
            switch (subclass) {
                case 0x0:
                    return "RAM Controller";
                case 0x1:
                    return "Flash Controller";
            }
            break;
            /* return "Memory"; */
        case 0x6: // Bridge
            switch (subclass) {
                case 0x0:
                    return "Host Bridge";
                case 0x1:
                    return "ISA Bridge";
                case 0x2:
                    return "EISA Bridge";
                case 0x3:
                    return "MCA Bridge";
                case 0x4:
                    return "PCI-to-PCI Bridge";
                case 0x5:
                    return "PCMCIA Bridge";
                case 0x6:
                    return "NuBus Bridge";
                case 0x7:
                    return "CardBus Bridge";
                case 0x8:
                    return "RACEway Bridge";
                case 0x9:
                    return "PCI-to-PCI Bridge";
                case 0x0A:
                    return "InfiniBand-to-PCI Host Bridge";
                default:
                    return "Unknown Bridge Controller";
            }
            break;
            /* return "Bridge"; */
        case 0x7: // Simple Communication
            /* return "Simple Comm"; */
            switch (subclass) {
                case 0x0:
                    return "Serial Controller";
                case 0x1:
                    return "Parallel Controller";
                case 0x2:
                    return "Multiport Serial Controller";
                case 0x3:
                    return "Modem";
                case 0x4:
                    return "IEEE 488.1/2 (GPIB) Controller";
                case 0x5:
                    return "Smart Card Controller ";
            }
            break;
        case 0x8: // Base System Peripheral
            /* return "Base System Periph"; */
            switch (subclass) {
                case 0x0:
                    return "PIC";
                case 0x1:
                    return "DMA Controller";
                case 0x2:
                    return "Timer";
                case 0x3:
                    return "RTC Controller";
                case 0x4:
                    return "PCI Hot-Plug Controller";
                case 0x5:
                    return "SD Host Controller";
                case 0x6:
                    return "IOMMU";
            }
            break;
        case 0x9: // Input Device
            switch (subclass) {
                case 0x0:
                    return "Keyboard Controller";
                case 0x1:
                    return "Digitizer Pen";
                case 0x2:
                    return "Mouse Controller";
                case 0x3:
                    return "Scanner Controller";
                case 0x4:
                    return "Gameport Controller";
            }
            break;
            /* return "Input Device"; */
        case 0xB: // Processor
            switch (subclass) {
                case 0x0:
                    return "386";
                case 0x1:
                    return "486";
                case 0x2:
                    return "Pentium";
                case 0x3:
                    return "Pentium Pro";
                case 0x10:
                    return "Alpha";
                case 0x20:
                    return "PowerPC";
                case 0x30:
                    return "MIPS";
                case 0x40:
                    return "Co-Processor";
            }
            break;
            /* return "Processor"; */
        case 0xC: // Serial Bus
            switch (subclass) {
                case 0x0:
                    return "FireWire (IEEE 1394) Controller";
                case 0x1:
                    return "ACCESS Bus Controller";
                case 0x2:
                    return "SSA";
                case 0x3:
                    return "USB Controller";
                case 0x4:
                    return "Fibre Channel";
                case 0x5:
                    return "SMBus Controller";
                case 0x6:
                    return "InfiniBand Controller";
                case 0x7:
                    return "IPMI Interface";
                case 0x8:
                    return "SERCOS Interface (IEC 61491)";
                case 0x9:
                    return "CANbus Controller";
            }
            break;
            /* return "Serial Bus"; */
    }
    return "Unknown";
}

static inline uint32_t format_address(uint8_t bus, uint8_t device, uint8_t func, uint8_t reg)
{
    return (1 << 31) | (bus << 16) | ((device & 0x1f) << 11) | ((func & 0x7) << 8) | (reg & 0xFC);
}

static uint32_t read_reg32(uint8_t bus, uint8_t device, uint8_t func, uint8_t reg)
{
    uint32_t address = format_address(bus, device, func, reg);

    portio_outl(PCI_CONFIG_ADDRESS, address);
    return portio_inl(PCI_CONFIG_DATA);
}

static void write_reg32(uint8_t bus, uint8_t device, uint8_t func, uint8_t reg, uint32_t val)
{
    uint32_t address = format_address(bus, device, func, reg);
    portio_outl(PCI_CONFIG_ADDRESS, address);
    portio_outl(PCI_CONFIG_DATA, val);
}

uint32_t pci_read_reg32(const struct PciEntry *entry, uint8_t reg)
{
    return read_reg32(entry->bus, entry->device, entry->func, reg);
}

void pci_write_reg32(const struct PciEntry *entry, uint8_t reg, uint32_t val)
{
    write_reg32(entry->bus, entry->device, entry->func, reg, val);
}

void pci_foreach(PciEntryCallback cb)
{
    for (int bus = 0; bus < 256; bus++) {
        for (int device = 0; device < 32; device++) {
            for (int func = 0; func < 8; func++) {
                uint32_t device_vendor = read_reg32(bus, device, func, PCI_REG_DEVICE_VENDOR);

                if (device_vendor == 0xffffffff) {
                    continue;
                }

                current_entry.bus    = bus;
                current_entry.device = device;
                current_entry.func   = func;

                current_entry.vendorId = device_vendor & 0xffff;
                current_entry.deviceId = (device_vendor >> 16) & 0xffff;

                uint32_t class_subclass_progif_revision =
                    pci_read_reg32(&current_entry, PCI_REG_CLASS_SUBCLASS_PROGIF_REVISION);
                uint8_t header_type_byte =
                    (pci_read_reg32(&current_entry, PCI_REG_BIST_HEADER_LATENCY_CLSIZE) >> 16) & 0xff;
                current_entry.header_type = header_type_byte & 0x7f;
                current_entry.mf          = (header_type_byte >> 7) & 1;

                current_entry.class_code = (class_subclass_progif_revision >> 24) & 0xff;
                current_entry.subclass   = (class_subclass_progif_revision >> 16) & 0xff;
                current_entry.progIf     = (class_subclass_progif_revision >> 8) & 0xff;
                current_entry.revisionId = class_subclass_progif_revision & 0xff;

                current_entry.revisionId = class_subclass_progif_revision & 0xff;
                uint16_t intpin_intline  = (pci_read_reg32(&current_entry, PCI_REG_X_X_INTPIN_INTLINE) >> 16) & 0xffff;
                current_entry.interrupt_pin  = (intpin_intline >> 8) & 0xff;
                current_entry.interrupt_line = intpin_intline & 0xff;

                for (int barIdx = 0; barIdx < 6; barIdx++) {
                    uint8_t  bar_address = PCI_REG_BAR_BASE + 4 * barIdx;
                    uint32_t bar         = pci_read_reg32(&current_entry, bar_address);

                    int      is_io     = bar & 0x1;
                    uint32_t addr_mask = 0xfffffff0;
                    if (is_io) {
                        addr_mask = 0xfffffffc;
                    }

                    pci_write_reg32(&current_entry, bar_address, 0xffffffff);
                    uint32_t bar_size = pci_read_reg32(&current_entry, bar_address);
                    bar_size          = ~(addr_mask & bar_size) + 1;
                    // restore original value
                    pci_write_reg32(&current_entry, bar_address, bar);

                    struct PciBar *current_bar = &current_entry.bars[barIdx];

                    if (bar_size == 0) {
                        current_bar->type = BAR_TYPE_NONE;
                    } else {
                        current_bar->size = bar_size;
                        if (is_io) {
                            current_bar->type = BAR_TYPE_IO;
                            current_bar->addr = bar & 0xfffffffc;
                        } else {
                            current_bar->type         = BAR_TYPE_MEM;
                            current_bar->addr         = bar & 0xfffffff0;
                            current_bar->prefetchable = (bar >> 3) != 0;
                            current_bar->mem_type     = (bar >> 1) & 0x3;
                        }
                    }
                }

                cb(&current_entry);
            }
        }
    }
}
