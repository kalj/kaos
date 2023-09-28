#include "kaos.h"

#include "tty.h"
#include "uart.h"

static bool to_uart = TRUE;
static bool to_tty = FALSE;

void kaos_setup_stdout(bool uart, bool tty)
{
    to_uart = uart;
    to_tty = tty;
}

void kaos_puts(const char* str)
{
    if(to_uart)
    {
        uart_puts(str);
    }
    if(to_tty)
    {
        tty_puts(str);
    }
}
