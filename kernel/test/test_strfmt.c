#include "strfmt.h"

#include <stdio.h>

void kaos_print_error_message_and_hang(const char *msg, const char *file, int line)
{
    printf("Panic at %s:%d - %s\n", file, line, msg);
    while (1) {
    }
}

int main()
{
    char buf[100];
    char *bufptr;
    int buflen;

    bufptr = buf;
    buflen = sizeof(buf);
    append_str(&bufptr, &buflen, "Normal string");
    puts(buf);

    bufptr = buf;
    buflen = sizeof(buf);
    append_str(&bufptr, &buflen, "Hex u8: ");
    append_u8_hex(&bufptr, &buflen, 0x83);
    puts(buf);

    bufptr = buf;
    buflen = sizeof(buf);
    append_str(&bufptr, &buflen, "Hex u16: ");
    append_u16_hex(&bufptr, &buflen, 0xbaba);
    puts(buf);

    bufptr = buf;
    buflen = sizeof(buf);
    append_str(&bufptr, &buflen, "Hex u32: ");
    append_u32_hex(&bufptr, &buflen, 0xDeadBeef);
    puts(buf);

    bufptr = buf;
    buflen = sizeof(buf);
    append_str(&bufptr, &buflen, "Dec s32: ");
    append_s32_dec(&bufptr, &buflen, 1);
    puts(buf);

    bufptr = buf;
    buflen = sizeof(buf);
    append_str(&bufptr, &buflen, "Dec s32: ");
    append_s32_dec(&bufptr, &buflen, -12);
    puts(buf);

    bufptr = buf;
    buflen = sizeof(buf);
    append_str(&bufptr, &buflen, "Dec s32: ");
    append_s32_dec(&bufptr, &buflen, 123456789);
    puts(buf);

    strfmt_snprintf(buf, 100, "Test snprintf with string: %s", "arg string");
    puts(buf);

    strfmt_snprintf(buf, 100, "Test snprintf with percentage %%");
    puts(buf);

    strfmt_snprintf(buf, 100, "Test snprintf with 6 u32: %l %l %l %l %l %l", 1, 2, 3, 0x1f, 0x8765, 0xdeadbeef);
    puts(buf);

    strfmt_snprintf(buf,
                    100,
                    "Test snprintf with %%d (%d), %%b (%b), %%w (%w), %%l (%l)",
                    (int)123,
                    0x1f,
                    0x8765,
                    0xdeadbeef);
    puts(buf);

    return 0;
}