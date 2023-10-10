#ifndef BIOS_SYSTEM_INFO_H
#define BIOS_SYSTEM_INFO_H

#include "kaos_int.h"

struct __attribute__((__packed__)) MemoryMapEntry {
    uint64_t base;
    uint64_t size;
    uint32_t type;
    uint32_t extra;
};

#define LOWMEM_SIZE_KB   (*(uint16_t *)0x500)
#define MEMORY_MAP_NUM   (*(uint16_t *)0x502)
#define MEMORY_MAP_ARRAY ((struct MemoryMapEntry *)0x504)

#endif /* BIOS_SYSTEM_INFO_H */
