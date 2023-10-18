#ifndef FLOPPY_H
#define FLOPPY_H

#include "kaos_int.h"

int floppy_init(void);

int floppy_read(uint8_t *dst, int lba, int n_bytes);

#endif /* FLOPPY_H */
