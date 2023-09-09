
char *vga_memory = (char *)0xB8000;
int cursor_position=80*16 + 23;

void print_char(char c)
{
    vga_memory[cursor_position*2] = c;
    vga_memory[cursor_position*2+1] = 0x0a;
    cursor_position++;
}

void print_str(const char *msg)
{
    while(*msg != 0)
    {
        print_char(*msg);
        msg++;
    }
}

void hang()
{
    while(1) {
        asm volatile("hlt");
    }
}

void kmain()
{
    const char *msg ="Hello from kernel.c!";
    print_str(msg);

    hang();
}
