#ifndef KAOS_H
#define KAOS_H

#include "kaos_bool.h"

void kaos_setup_stdout(bool uart, bool tty);

void kaos_puts(const char* str);

#endif /* KAOS_H */    
