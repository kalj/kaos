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
    int  res;

    res = strfmt_u8_hex(buf, sizeof(buf), 0x83);
    if (res < 0) return res;
    printf("Hex u8: %s\n", buf);

    res = strfmt_u16_hex(buf, sizeof(buf), 0xbaba);
    if (res < 0) return res;
    printf("Hex u16: %s\n", buf);

    res = strfmt_u32_hex(buf, sizeof(buf), 0xDeadBeef);
    if (res < 0) return res;
    printf("Hex u32: %s\n", buf);

    res = strfmt_s32_dec(buf, sizeof(buf), 1);
    if (res < 0) return res;
    printf("Dec s32 of 1: %s\n", buf);

    res = strfmt_s32_dec(buf, sizeof(buf), -12);
    if (res < 0) return res;
    printf("Dec s32 of -12: %s\n", buf);

    res = strfmt_s32_dec(buf, sizeof(buf), 123456789);
    if (res < 0) return res;
    printf("Dec s32 of 123456789: %s\n", buf);

    res = strfmt_snprintf(buf, 100, "Test snprintf with string: %s", "arg string");
    if (res < 0) return res;
    puts(buf);

    res = strfmt_snprintf(buf, 100, "Test snprintf with percentage %%");
    if (res < 0) return res;
    puts(buf);

    res = strfmt_snprintf(buf, 100, "Test snprintf with 6 u32: %l %l %l %l %l %l", 1, 2, 3, 0x1f, 0x8765, 0xdeadbeef);
    if (res < 0) return res;
    puts(buf);

    res = strfmt_snprintf(buf,
                          100,
                          "Test snprintf with %%d (%d), %%b (%b), %%w (%w), %%l (%l)",
                          (int)123,
                          0x1f,
                          0x8765,
                          0xdeadbeef);
    if (res < 0) return res;
    puts(buf);

    return 0;
}
