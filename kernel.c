

void print_char(char c)
{
  asm volatile("    \n\t\
    mov %0, %%al    \n\t\
    mov $0x0e, %%ah  \n\t\
    mov $0x00, %%bh  \n\t\
    mov $0x09, %%bl  \n\t\
    int $0x10"
               : /* no outputs */
               : [c] "rm" (c)
               : "ax", "bx");
}

/* void print_str(const char *msg) */
/* { */
/*     while(*msg != 0) */
/*     { */
/*         print_char(*msg); */
/*         msg++; */
/*     } */
/* } */

/* /\* __attribute__((noreturn)) *\/ */
/* void hang() */
/* { */
/*     while(1) { */
/*     asm volatile("hlt"); */
/*     } */
/* } */


__attribute__((section(".text.start"), naked))
void start()
{
    /* hang(); */
    const char *msg ="Hello from kernel.c!\r\n";

    for(int i=0; i<10; i++)
    {
        print_char(msg[i]);
    }

    while(1) {
    asm volatile("hlt");
    }
}
