#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include "kaos_int.h"

typedef struct __attribute__((__packed__)) {
    uint64_t base;
    uint64_t size;
    uint32_t type;
    uint32_t extra;
} MemoryMapEntry;


#define MEMORY_MAP_ARRAY ((MemoryMapEntry *)0x7e00)
#define MEMORY_MAP_NUM 5



#endif /* MEMORY_MAP_H */
