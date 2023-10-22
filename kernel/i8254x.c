#include "i8254x.h"

#include "kaos.h"
#include "panic.h"
#include "pci.h"

#define EECD (0x00010)

void i8254x_init(const struct PciEntry *pci_entry)
{
    /* (void)pci_entry; */
    /* (void)pci_bus; */
    /* (void)pci_dev; */
    /* (void)pci_func; */
    if (pci_entry->bars[0].type != BAR_TYPE_MEM || pci_entry->bars[0].mem_type != 0) {
        KAOS_PANIC("Unexpected bar0");
    }

    // enable mem, io, and bus mastering
    uint32_t status_command = pci_read_reg32(pci_entry, 4);
    status_command |= 0x7;
    pci_write_reg32(pci_entry, 4, status_command);

    volatile uint32_t *eecd = (volatile uint32_t *)(pci_entry->bars[0].addr + EECD);

    uint32_t eecd_val = *eecd;

    kaos_printf("[i8254x] eecd initial value: %l\n", eecd_val);
    *eecd = eecd_val | 0x7; // set SK, CS, DI
}
