#include "irq.h"

#include "kaos_int.h"
#include "tty.h"

#define NUM_IDT_ARRAY_ENTRIES 256

typedef struct {
    uint16_t offset_1;       // offset bits 0..15
    uint16_t selector;       // a code segment selector in GDT or LDT
    uint8_t zero;            // unused, set to 0
    uint8_t type_attributes; // gate type, dpl, and p fields
    uint16_t offset_2;       // offset bits 16..31
} __attribute__((packed)) InterruptDescriptor;


// this should contain NUM_IDT_ARRAY_ENTRIES distinct entries, each of which 
__attribute__((aligned(0x10))) static InterruptDescriptor idt[NUM_IDT_ARRAY_ENTRIES];

static void setup_interrupt(int num, void *irq, uint8_t type) {
    InterruptDescriptor *desc = &idt[num];
    desc->offset_1            = ((uint32_t)irq) & 0xFFFF;
    desc->selector            = 0x0008;
    desc->zero                = 0x0;
    desc->type_attributes     = type;
    desc->offset_2            = (((uint32_t)irq) >> 16) & 0xFFFF;
}

static struct {
    uint16_t size;
    uint32_t base;
} __attribute__((__packed__)) idtr = {.size = sizeof(idt) - 1, .base = (uint32_t)&idt};

static __attribute__((interrupt)) void stub_handler(void *irq_frame) {
}

static __attribute__((interrupt)) void keyb_handler(void *irq_frame) {
    /* tty_print_str("Banan!\n"); */
}

void irq_init() {

    for (int i = 0; i < NUM_IDT_ARRAY_ENTRIES; i++) {
        setup_interrupt(i, stub_handler, 0x8E);
    }

    setup_interrupt(9, keyb_handler, 0x8E);

    asm volatile("lidt %0" : : "m"(idtr));
}
