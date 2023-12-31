#ifndef MEMCPY_H
#define MEMCPY_H

#include "kaos_int.h"

void *memcpy(void *dst, const void *src, uint32_t len);
void *memset(void *dst, int v, uint32_t len);

#endif /* MEMCPY_H */
