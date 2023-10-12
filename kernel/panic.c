#include "panic.h"

#include "kaos.h"
#include "strfmt.h"

void kaos_print_error_message_and_hang(const char *msg, const char *file, int line)
{
    kaos_puts("Panic at ");
    kaos_puts(file);
    kaos_puts(":");
    char buf[80];
    strfmt_s32_dec(buf, 80, line);
    kaos_puts(buf);
    kaos_puts(": ");
    kaos_puts(msg);

    // hang
    while (1) {
        asm volatile("hlt");
    }
}
