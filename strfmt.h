#ifndef STRFMT_H
#define STRFMT_H

#include "kaos_int.h"

// returns number of chars written, including null byte, or 0 if it didn't fit. 

int strfmt_u8_hex(char *buf, int buflen, uint8_t v);
int strfmt_u16_hex(char *buf, int buflen, uint16_t v);
int strfmt_u32_hex(char *buf, int buflen, uint32_t v);
int strfmt_u64_hex(char *buf, int buflen, uint64_t v);

int strfmt_s32_dec(char *buf, int buflen, int32_t v);

#endif /* STRFMT_H */
