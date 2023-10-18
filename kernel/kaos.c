#include "kaos.h"

#include "strfmt.h"
#include "tty.h"
#include "uart.h"

static bool to_uart = TRUE;
static bool to_tty  = FALSE;

void kaos_setup_stdout(bool uart, bool tty)
{
    to_uart = uart;
    to_tty  = tty;
}

void kaos_puts(const char *str)
{
    if (to_uart) {
        uart_puts(str);
    }
    if (to_tty) {
        tty_puts(str);
    }
}

static char printbuf[120];
void kaos_printf(const char *fmt, ...)
{
    __builtin_va_list va;
    __builtin_va_start(va, fmt);
    int ret = strfmt_vsnprintf(printbuf, sizeof(printbuf), fmt, va);
    __builtin_va_end(va);
    if (ret >= 0) {
        kaos_puts(printbuf);
    }
}
