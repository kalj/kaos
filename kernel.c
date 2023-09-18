#include "kaos_int.h"


char *vga_memory = (char *)0xB8000;
int cursor_position=0;

void clear_screen()
{
    for(int i=0; i<80*20; i++) {
        vga_memory[i*2] = 0;
        vga_memory[i*2+1] = 0;
    }
}

void print_char(char c, char style)
{
    if(c == '\n') {
        cursor_position = (cursor_position/80 + 1)*80;
    } else {
        vga_memory[cursor_position*2] = c;
        vga_memory[cursor_position*2+1] = style;
        cursor_position++;
    }
}

void print_str(const char *msg)
{
    while(*msg != 0)
    {
        print_char(*msg, 0x07);
        msg++;
    }
}

void print_hex4(uint8_t v)
{
    char c = v<10 ? '0'+v : 'a'+(v-10);
    print_char(c, 0x07);
}

void print_hex8(uint8_t v)
{
    print_hex4(v>>4);
    print_hex4(v&0xf);
}

#define PRINT_NIBBLES(v,n)                      \
    do {                                        \
        for(int i=(n-1); i>=0; i--) {           \
            print_hex4((v>>(i*4))&0xf);         \
        }                                       \
    }  while(0)

    

void print_hex16(uint16_t v)
{
    PRINT_NIBBLES(v,4);
}

void print_hex32(uint32_t v)
{
    PRINT_NIBBLES(v,8);
}

void print_hex64(uint64_t v)
{
    PRINT_NIBBLES(v,16);
}

void hang()
{
    while(1) {
        asm volatile("hlt");
    }
}


typedef struct __attribute__((__packed__))
{
    uint64_t base;
    uint64_t size;
    uint32_t type;
    uint32_t extra;
} MemoryMapEntry;

MemoryMapEntry *memory_map = (MemoryMapEntry *)0x7e00;

void print_mem_map()
{
    print_str("Base               Size               Type\n");
    for(int i=0; i<5; i++)
    {
        print_hex64(memory_map[i].base);
        print_str("   ");
        print_hex64(memory_map[i].size);
        print_str("   ");
        if(memory_map[i].type == 1) {
            print_str("M      ");
        } else if(memory_map[i].type == 2) {
            print_str("R      ");
        } else {
            print_str("?      ");
        }
        print_str("\n");
    }
}

void kmain()
{

    cursor_position = 80*16;
    print_str("Hello from kernel.c!\n");

    print_str("Memory map:\n");
    print_mem_map();

    hang();
}
