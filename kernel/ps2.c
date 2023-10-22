#include "ps2.h"

int ps2_reboot()
{
    for (int i = 0; i < PS2_N_TRIES; i++) {
        if ((portio_inb(PS2_STATUS_PORT) & PS2_STATUS_IBUF) == 0) {
            portio_outb(PS2_COMMAND_PORT, PS2_CMD_REBOOT);
            return 0;
        }
    }
    return -1;
}
