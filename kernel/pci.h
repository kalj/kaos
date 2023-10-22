#ifndef PCI_H
#define PCI_H

#include "kaos_bool.h"
#include "kaos_int.h"

#define PCI_REG_DEVICE_VENDOR                  0
#define PCI_REG_STATUS_COMMAND                 4
#define PCI_REG_COMMAND                        4
#define PCI_REG_STATUS                         6
#define PCI_REG_CLASS_SUBCLASS_PROGIF_REVISION 8
#define PCI_REG_REVISION                       8
#define PCI_REG_PROGIF                         9
#define PCI_REG_SUBCLASS                       10
#define PCI_REG_CLASS                          11
#define PCI_REG_BIST_HEADER_LATENCY_CLSIZE     12
#define PCI_REG_CLSIZE                         12
#define PCI_REG_LATENCY                        13
#define PCI_REG_HEADER                         14
#define PCI_REG_BIST                           15
#define PCI_REG_BAR_BASE                       16
#define PCI_REG_X_X_INTPIN_INTLINE             0x3c

enum BarType {
    BAR_TYPE_NONE,
    BAR_TYPE_MEM,
    BAR_TYPE_IO,
};

struct PciBar {
    enum BarType type;
    uint32_t     addr;
    uint32_t     size;
    int          mem_type;     // only applicable for mem type
    bool         prefetchable; // only applicable for mem type
};

struct PciEntry {
    uint8_t       bus;
    uint8_t       device;
    uint8_t       func;
    uint16_t      vendorId;
    uint16_t      deviceId;
    uint8_t       header_type;
    bool          mf;
    uint8_t       class_code;
    uint8_t       subclass;
    uint8_t       progIf;
    uint8_t       revisionId;
    uint8_t       interrupt_pin;
    uint8_t       interrupt_line;
    struct PciBar bars[6];
};

typedef void PciEntryCallback(const struct PciEntry *);

void pci_foreach(PciEntryCallback cb);

uint32_t pci_read_reg32(const struct PciEntry *entry, uint8_t reg);
void     pci_write_reg32(const struct PciEntry *entry, uint8_t reg, uint32_t val);

const char *pci_get_class_description(const struct PciEntry *entry);

#endif /* PCI_H */
