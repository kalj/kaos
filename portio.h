#ifndef PORTIO_H
#define PORTIO_H

#include "kaos_int.h"

static inline void portio_outb(uint16_t port, uint8_t val)
{
    asm volatile ("out %0, %1"
                  :
                  : "a"(val),
                    "Nd"(port)
                  : "memory" );
}

static inline uint8_t portio_inb(uint16_t port)
{
    uint8_t val;
    asm volatile ("in %1, %0"
                  : "=a"(val)
                  : "Nd"(port)
                  : "memory"
        );
                    
    return val;
}

static inline void portio_wait(void) {
    portio_outb(0x80, 0);
}


#endif /* PORTIO_H */
