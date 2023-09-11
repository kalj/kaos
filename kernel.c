
char *vga_memory = (char *)0xB8000;
int cursor_position=0;

void clear_screen()
{
    for(int i=0; i<80*20; i++) {
        vga_memory[i*2] = 0;
        vga_memory[i*2+1] = 0;
    }
}

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

    cursor_position = (cursor_position/80 + 1)*80;
}

void hang()
{
    while(1) {
        asm volatile("hlt");
    }
}

void kmain()
{

    clear_screen();
    print_str("Hello from kernel.c!");

    hang();
}
