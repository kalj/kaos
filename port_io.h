#ifndef PORT_IO_H
#define PORT_IO_H

#include "kaos_int.h"

static inline void outportb(uint16_t port, uint8_t val)
{
    asm volatile ("out %0, %1"
                  :
                  : "a"(val),
                    "Nd"(port)
                  : "memory" );
}

static inline uint8_t inportb(uint16_t port)
{
    uint8_t val;
    asm volatile ("in %1, %0"
                  : "=a"(val)
                  : "Nd"(port)
                  : "memory"
        );
                    
    return val;
}

#endif /* PORT_IO_H */
