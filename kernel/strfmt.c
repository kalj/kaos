#include "strfmt.h"

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
