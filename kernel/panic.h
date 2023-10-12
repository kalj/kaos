#ifndef PANIC_H
#define PANIC_H

void kaos_print_error_message_and_hang(const char *msg, const char *file, int line);

#define KAOS_PANIC(MSG) kaos_print_error_message_and_hang(MSG, __FILE__, __LINE__);

#endif /* PANIC_H */
