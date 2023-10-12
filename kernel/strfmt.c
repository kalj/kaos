#include "strfmt.h"

#include "memcpy.h"
#include "panic.h"

#define STRFMT_NIBBLE(n) ((n) < 10 ? '0' + (n) : 'a' + ((n)-10))

static inline int format_nibbles(char *buf, int buflen, int n_nibbles, uint64_t v)
{
    if (buflen < (n_nibbles + 1))
        return 0;
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

    if (v == 0)
        return 1;
    while (v > 0) {
        len++;
        v /= 10;
    }

    return len;
}

int strfmt_s32_dec(char *buf, int buflen, int32_t v)
{
    int nchars = count_dec_len(v);
    if (nchars + 1 > buflen)
        return 0;

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

void append_str(char **bufptr, int *buflen, const char *str)
{
    int len = strfmt_strlen(str);
    if (len > *buflen)
        return;
    memcpy(*bufptr, str, len);
    *bufptr += len;
    *buflen -= len;
}

void append_u8_hex(char **bufptr, int *buflen, uint8_t v)
{
    strfmt_u8_hex(*bufptr, *buflen, v);
    *bufptr += 2;
    *buflen -= 2;
}

void append_u16_hex(char **bufptr, int *buflen, uint16_t v)
{
    strfmt_u16_hex(*bufptr, *buflen, v);
    *bufptr += 4;
    *buflen -= 4;
}

void append_u32_hex(char **bufptr, int *buflen, uint32_t v)
{
    strfmt_u32_hex(*bufptr, *buflen, v);
    *bufptr += 8;
    *buflen -= 8;
}

void append_s32_dec(char **bufptr, int *buflen, int32_t v)
{
    int len = strfmt_s32_dec(*bufptr, *buflen, v);
    if (len == 0)
        return;
    *bufptr += (len - 1);
    *buflen -= (len - 1);
}

void strfmt_snprintf(char *buf, int buflen, const char *fmt, void **args)
{
    int argidx = 0;
    while (1) {
        if (*fmt == '\0') {
            break;
        } else if (*fmt == '%') {
            if (fmt[1] == '\0') {
                break;
            }

            if (fmt[1] == '%') {
                if (1 >= buflen) {
                    break;
                }

                *buf = '%';
                buf++;
                fmt += 2;
                buflen--;
            } else if (fmt[1] == 's') {
                const char *str = args[argidx];
                int len         = strfmt_strlen(str);
                if (len >= buflen) {
                    break;
                }
                append_str(&buf, &buflen, str);
                /* buf++; */
                /* buflen--; */
                fmt += 2;
                argidx++;
            } else if (fmt[1] == 'd') {
                unsigned long val = (unsigned long)args[argidx];
                int len           = count_dec_len(val);
                if (len >= buflen) {
                    break;
                }
                append_s32_dec(&buf, &buflen, val);
                fmt += 2;
                argidx++;
            } else if (fmt[1] == 'b') {
                uint8_t val = (uint8_t)(uint32_t)args[argidx];
                if (2 >= buflen) {
                    break;
                }
                append_u8_hex(&buf, &buflen, val);
                fmt += 2;
                argidx++;
            } else if (fmt[1] == 'w') {
                uint16_t val = (uint16_t)(uint32_t)args[argidx];
                if (4 >= buflen) {
                    break;
                }
                append_u16_hex(&buf, &buflen, val);
                fmt += 2;
                argidx++;
            } else if (fmt[1] == 'l') {
                uint32_t val = (uint32_t)args[argidx];
                if (8 >= buflen) {
                    break;
                }
                append_u32_hex(&buf, &buflen, val);
                fmt += 2;
                argidx++;
            } else {
                char errstr[80];
                char *bufptr = errstr;
                int buflen   = sizeof(errstr);
                append_str(&bufptr, &buflen, "unrecognized format char: ");
                *bufptr++ = fmt[1];
                *bufptr++ = '\0';

                KAOS_PANIC(errstr);
            }
            // handle formatting
        } else {
            if (1 >= buflen) {
                break;
            }
            *buf = *fmt;
            buf++;
            fmt++;
            buflen--;
        }
    }
    *buf = '\0';
}
