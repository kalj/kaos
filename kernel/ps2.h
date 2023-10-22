#ifndef PS2_H
#define PS2_H

#include "kaos_int.h"
#include "portio.h"

#define PS2_DATA_PORT      0x60
#define PS2_STATUS_PORT    0x64
#define PS2_COMMAND_PORT   0x64
#define PS2_STATUS_OBUF    0x01
#define PS2_STATUS_IBUF    0x02
#define PS2_CMD_READ_CONF  0x20
#define PS2_CMD_WRITE_CONF 0x60
#define PS2_CMD_REBOOT     0xFE

#define PS2_N_TRIES 100

static inline int ps2_conf_read(void)
{
    portio_outb(PS2_COMMAND_PORT, PS2_CMD_READ_CONF);

    // waiting for output ready
    for (int i = 0; i < PS2_N_TRIES; i++) {
        if (portio_inb(PS2_STATUS_PORT) & PS2_STATUS_OBUF) {
            return portio_inb(PS2_DATA_PORT);
        }
    }
    return -1;
}

static inline int ps2_conf_write(uint8_t c)
{
    portio_outb(PS2_COMMAND_PORT, PS2_CMD_WRITE_CONF);

    // waiting for input ready
    for (int i = 0; i < PS2_N_TRIES; i++) {
        if ((portio_inb(PS2_STATUS_PORT) & PS2_STATUS_IBUF) == 0) {
            portio_outb(PS2_DATA_PORT, c);
            return 0;
        }
    }
    return -1;
}

int ps2_reboot(void);

static inline uint8_t ps2_get_scancode(void)
{
    if (portio_inb(PS2_STATUS_PORT) & PS2_STATUS_OBUF) {
        return portio_inb(PS2_DATA_PORT);
    }
    return 0;
}

#endif /* PS2_H */
