#ifndef FLOPPY_H
#define FLOPPY_H

int floppy_init(void);

int floppy_read(void *dst, int lba, int n_bytes);

#endif /* FLOPPY_H */
