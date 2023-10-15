#include "memcpy.h"

void *memcpy(void *dst, const void *src, uint32_t len)
{
    uint8_t *bdst       = dst;
    const uint8_t *bsrc = src;
    for (int i = 0; i < len; i++) {
        bdst[i] = bsrc[i];
    }
    return dst;
}
void *memset(void *dst, int v, uint32_t len)
{
    uint8_t *bdst = dst;
    for (int i = 0; i < len; i++) {
        bdst[i] = v & 0xff;
    }
    return dst;
}
