#ifndef STRFMT_H
#define STRFMT_H

#include "kaos_int.h"

// returns number of chars written, including null byte, or 0 if it didn't fit.

int strfmt_u8_hex(char *buf, int buflen, uint8_t v);
int strfmt_u16_hex(char *buf, int buflen, uint16_t v);
int strfmt_u32_hex(char *buf, int buflen, uint32_t v);
int strfmt_u64_hex(char *buf, int buflen, uint64_t v);
int strfmt_s32_dec(char *buf, int buflen, int32_t v);

int strfmt_strlen(const char *str);

void append_str(char **bufptr, int *buflen, const char *str);
void append_u8_hex(char **bufptr, int *buflen, uint8_t v);
void append_u16_hex(char **bufptr, int *buflen, uint16_t v);
void append_u32_hex(char **bufptr, int *buflen, uint32_t v);
void append_s32_dec(char **bufptr, int *buflen, int32_t v);

void strfmt_snprintf(char *buf, int buflen, const char *fmt, ...);

#endif /* STRFMT_H */
