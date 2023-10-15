#include "strfmt.h"

#include "memcpy.h"

#define STRFMT_NIBBLE(n) ((n) < 10 ? '0' + (n) : 'a' + ((n)-10))

static inline int format_nibbles(char *buf, int buflen, int n_nibbles, uint64_t v)
{
    if (buflen < (n_nibbles + 1)) return 0;
    for (int i = 0; i < n_nibbles; i++) {
        buf[i] = STRFMT_NIBBLE((v >> (4 * (n_nibbles - 1 - i))) & 0xf);
    }
    buf[n_nibbles] = '\0';

    return n_nibbles + 1;
}

int strfmt_u8_hex(char *buf, int buflen, uint8_t v)
{
    return format_nibbles(buf, buflen, 2, v);
}

int strfmt_u16_hex(char *buf, int buflen, uint16_t v)
{
    return format_nibbles(buf, buflen, 4, v);
}

int strfmt_u32_hex(char *buf, int buflen, uint32_t v)
{
    return format_nibbles(buf, buflen, 8, v);
}

int strfmt_u64_hex(char *buf, int buflen, uint64_t v)
{
    return format_nibbles(buf, buflen, 16, v);
}

static int count_dec_len(int32_t v)
{
    int len = 0;
    if (v < 0) {
        len++;
        v = -v;
    }

    if (v == 0) return 1;
    while (v > 0) {
        len++;
        v /= 10;
    }

    return len;
}

int strfmt_s32_dec(char *buf, int buflen, int32_t v)
{
    int nchars = count_dec_len(v);
    if (nchars + 1 > buflen) return 0;

    char *orig_buf = buf;

    if (v < 0) {
        v      = -v;
        *buf++ = '-';
        nchars--;
    }

    if (v == 0) {
        *buf++ = '0';
    } else {

        for (int i = 0; i < nchars; i++) {
            buf[nchars - 1 - i] = '0' + (v % 10);
            v /= 10;
        }
        buf += nchars;
    }

    // write last char
    *buf++ = '\0';

    return buf - orig_buf;
}

int strfmt_strlen(const char *str)
{
    int len = 0;
    while (*str++ != '\0') {
        len++;
    }
    return len;
}

static void append_str(char **bufptr, int *buflen, const char *str)
{
    int len = strfmt_strlen(str);
    if (len > *buflen) return;
    memcpy(*bufptr, str, len);
    *bufptr += len;
    *buflen -= len;
    **bufptr = '\0';
}

static void append_u8_hex(char **bufptr, int *buflen, uint8_t v)
{
    strfmt_u8_hex(*bufptr, *buflen, v);
    *bufptr += 2;
    *buflen -= 2;
}

static void append_u16_hex(char **bufptr, int *buflen, uint16_t v)
{
    strfmt_u16_hex(*bufptr, *buflen, v);
    *bufptr += 4;
    *buflen -= 4;
}

static void append_u32_hex(char **bufptr, int *buflen, uint32_t v)
{
    strfmt_u32_hex(*bufptr, *buflen, v);
    *bufptr += 8;
    *buflen -= 8;
}

static void append_u64_hex(char **bufptr, int *buflen, uint64_t v)
{
    strfmt_u64_hex(*bufptr, *buflen, v);
    *bufptr += 16;
    *buflen -= 16;
}

static void append_s32_dec(char **bufptr, int *buflen, int32_t v)
{
    int len = strfmt_s32_dec(*bufptr, *buflen, v);
    if (len == 0) return;
    *bufptr += (len - 1);
    *buflen -= (len - 1);
}

int strfmt_snprintf(char *buf, int buflen, const char *fmt, ...)
{
    __builtin_va_list va;
    __builtin_va_start(va, fmt);
    char *buf_start = buf;

    while (1) {
        if (*fmt == '\0') {
            break;
        } else if (*fmt == '%') {
            if (fmt[1] == '%') {
                if (1 >= buflen) {
                    return STRFMT_ERROR_BUFFER_OVERFLOW;
                }

                *buf = '%';
                buf++;
                fmt += 2;
                buflen--;
            } else if (fmt[1] == 'c') {
                char c = __builtin_va_arg(va, signed int);
                if (1 >= buflen) {
                    return STRFMT_ERROR_BUFFER_OVERFLOW;
                }
                *buf = c;
                buf++;
                fmt += 2;
                buflen--;
            } else if (fmt[1] == 's') {
                const char *str = __builtin_va_arg(va, const char *);
                int len         = strfmt_strlen(str);
                if (len >= buflen) {
                    return STRFMT_ERROR_BUFFER_OVERFLOW;
                }
                append_str(&buf, &buflen, str);
                fmt += 2;
            } else if (fmt[1] == 'd') {
                unsigned long val = __builtin_va_arg(va, unsigned long);
                int len           = count_dec_len(val);
                if (len >= buflen) {
                    return STRFMT_ERROR_BUFFER_OVERFLOW;
                }
                append_s32_dec(&buf, &buflen, val);
                fmt += 2;
            } else if (fmt[1] == 'b') {
                uint8_t val = __builtin_va_arg(va, unsigned int);
                if (2 >= buflen) {
                    return STRFMT_ERROR_BUFFER_OVERFLOW;
                }
                append_u8_hex(&buf, &buflen, val);
                fmt += 2;
            } else if (fmt[1] == 'w') {
                uint16_t val = __builtin_va_arg(va, unsigned int);
                if (4 >= buflen) {
                    return STRFMT_ERROR_BUFFER_OVERFLOW;
                }
                append_u16_hex(&buf, &buflen, val);
                fmt += 2;
            } else if (fmt[1] == 'l') {
                uint32_t val = __builtin_va_arg(va, unsigned int);
                if (8 >= buflen) {
                    return STRFMT_ERROR_BUFFER_OVERFLOW;
                }
                append_u32_hex(&buf, &buflen, val);
                fmt += 2;
            } else if (fmt[1] == 'q') {
                uint64_t val = __builtin_va_arg(va, uint64_t);
                if (16 >= buflen) {
                    return STRFMT_ERROR_BUFFER_OVERFLOW;
                }
                append_u64_hex(&buf, &buflen, val);
                fmt += 2;
            } else {
                return STRFMT_ERROR_UNKNOWN_FMTCHAR;
            }
            // handle formatting
        } else {
            if (1 >= buflen) {
                return STRFMT_ERROR_BUFFER_OVERFLOW;
            }
            *buf = *fmt;
            buf++;
            fmt++;
            buflen--;
        }
    }
    *buf = '\0';
    __builtin_va_end(va);

    return buf - buf_start;
}
