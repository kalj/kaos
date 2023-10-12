#ifndef PORTIO_H
#define PORTIO_H

#include "kaos_int.h"

static inline void portio_outb(uint16_t port, uint8_t val)
{
    asm volatile("out %0, %1"
                 :
                 : "a"(val), "Nd"(port)
                 : "memory");
}

static inline uint8_t portio_inb(uint16_t port)
{
    uint8_t val;
    asm volatile("in %1, %0"
                 : "=a"(val)
                 : "Nd"(port)
                 : "memory");

    return val;
}

static inline void portio_outl(uint16_t port, uint32_t val)
{
    asm volatile("outl %0, %1"
                 :
                 : "a"(val), "Nd"(port)
                 : "memory");
}

static inline uint32_t portio_inl(uint16_t port)
{
    uint32_t res;
    asm volatile("inl %1, %0"
                 : "=a"(res)
                 : "Nd"(port)
                 : "memory");
    return res;
}

static inline void portio_wait(void)
{
    portio_outb(0x80, 0);
}

#endif /* PORTIO_H */
