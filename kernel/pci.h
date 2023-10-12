#ifndef PCI_H
#define PCI_H

#include "kaos_bool.h"
#include "kaos_int.h"

struct PciEntry {
    uint8_t bus;
    uint8_t device;
    uint8_t func;
    uint16_t vendorId;
    uint16_t deviceId;
    uint16_t status;
    uint16_t command;
    uint8_t header_type;
    bool mf;
    uint8_t class;
    uint8_t subclass;
    uint8_t progIf;
    uint8_t revisionId;
};

typedef void PciEntryCallback(const struct PciEntry *);

void pci_foreach(PciEntryCallback cb);

uint32_t pci_read_reg32(uint8_t bus, uint8_t device, uint8_t func, uint8_t reg);

void pci_write_reg32(uint8_t bus, uint8_t device, uint8_t func, uint8_t reg, uint32_t val);

const char *pci_get_class_description(const struct PciEntry *entry);

#endif /* PCI_H */
