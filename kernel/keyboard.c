#include "keyboard.h"

#include "irq.h"
#include "kaos.h"
#include "kaos_bool.h"
#include "panic.h"
#include "pic.h"
#include "portio.h"
#include "ps2.h"

#define IRQ_VEC_KEYBOARD 33

enum Keycode {
    KEYCODE_ESCAPE     = 0x01,
    KEYCODE_1          = 0x02,
    KEYCODE_2          = 0x03,
    KEYCODE_3          = 0x04,
    KEYCODE_4          = 0x05,
    KEYCODE_5          = 0x06,
    KEYCODE_6          = 0x07,
    KEYCODE_7          = 0x08,
    KEYCODE_8          = 0x09,
    KEYCODE_9          = 0x0A,
    KEYCODE_0          = 0x0B,
    KEYCODE_MINUS      = 0x0C,
    KEYCODE_EQUAL      = 0x0D,
    KEYCODE_BACKSPACE  = 0x0E,
    KEYCODE_TAB        = 0x0F,
    KEYCODE_Q          = 0x10,
    KEYCODE_W          = 0x11,
    KEYCODE_E          = 0x12,
    KEYCODE_R          = 0x13,
    KEYCODE_T          = 0x14,
    KEYCODE_Y          = 0x15,
    KEYCODE_U          = 0x16,
    KEYCODE_I          = 0x17,
    KEYCODE_O          = 0x18,
    KEYCODE_P          = 0x19,
    KEYCODE_LBRACKET   = 0x1A,
    KEYCODE_RBRACKET   = 0x1B,
    KEYCODE_ENTER      = 0x1C,
    KEYCODE_LCONTROL   = 0x1D,
    KEYCODE_A          = 0x1E,
    KEYCODE_S          = 0x1F,
    KEYCODE_D          = 0x20,
    KEYCODE_F          = 0x21,
    KEYCODE_G          = 0x22,
    KEYCODE_H          = 0x23,
    KEYCODE_J          = 0x24,
    KEYCODE_K          = 0x25,
    KEYCODE_L          = 0x26,
    KEYCODE_SEMICOLON  = 0x27,
    KEYCODE_SQUOTE     = 0x28,
    KEYCODE_BACKTICK   = 0x29,
    KEYCODE_LSHIFT     = 0x2A,
    KEYCODE_BACKSLASH  = 0x2B,
    KEYCODE_Z          = 0x2C,
    KEYCODE_X          = 0x2D,
    KEYCODE_C          = 0x2E,
    KEYCODE_V          = 0x2F,
    KEYCODE_B          = 0x30,
    KEYCODE_N          = 0x31,
    KEYCODE_M          = 0x32,
    KEYCODE_COMMA      = 0x33,
    KEYCODE_PERIOD     = 0x34,
    KEYCODE_SLASH      = 0x35,
    KEYCODE_RSHIFT     = 0x36,
    KEYCODE_KPASTERISK = 0x37,
    KEYCODE_LALT       = 0x38,
    KEYCODE_SPACE      = 0x39,
    KEYCODE_CAPS       = 0x3A,
    KEYCODE_F1         = 0x3B,
    KEYCODE_F2         = 0x3C,
    KEYCODE_F3         = 0x3D,
    KEYCODE_F4         = 0x3E,
    KEYCODE_F5         = 0x3F,
    KEYCODE_F6         = 0x40,
    KEYCODE_F7         = 0x41,
    KEYCODE_F8         = 0x42,
    KEYCODE_F9         = 0x43,
    KEYCODE_F10        = 0x44,
    KEYCODE_NUMLOCK    = 0x45,
    KEYCODE_SCRLOCK    = 0x46,
    KEYCODE_KP7        = 0x47,
    KEYCODE_KP8        = 0x48,
    KEYCODE_KP9        = 0x49,
    KEYCODE_KPMINUS    = 0x4A,
    KEYCODE_KP4        = 0x4B,
    KEYCODE_KP5        = 0x4C,
    KEYCODE_KP6        = 0x4D,
    KEYCODE_KPPLUS     = 0x4E,
    KEYCODE_KP1        = 0x4F,
    KEYCODE_KP2        = 0x50,
    KEYCODE_KP3        = 0x51,
    KEYCODE_KP0        = 0x52,
    KEYCODE_KPPERIOD   = 0x53,
    // 0x54
    // 0x55
    // 0x56
    KEYCODE_F11 = 0x57,
    KEYCODE_F12 = 0x58,
};

/* static int scancode_to_keycode[128] = { */
/*     0, */
/* }; */

// modifiers state
static bool rshift_active = FALSE;
static bool lshift_active = FALSE;
static bool lalt_active   = FALSE;
/* static bool ralt_active = FALSE; */
static bool lcontrol_active = FALSE;
/* static bool rcontrol_active = FALSE; */

static char keymap[][2] = {
    {0, 0},       // ?
    {0, 0},       // ESCAPE
    {'1', '!'},   // KEYCODE_1
    {'2', '@'},   // KEYCODE_2
    {'3', '#'},   // KEYCODE_3
    {'4', '$'},   // KEYCODE_4
    {'5', '%'},   // KEYCODE_5
    {'6', '^'},   // KEYCODE_6
    {'7', '&'},   // KEYCODE_7
    {'8', '*'},   // KEYCODE_8
    {'9', '('},   // KEYCODE_9
    {'0', ')'},   // KEYCODE_0
    {'-', '_'},   // KEYCODE_MINUS
    {'=', '+'},   // KEYCODE_EQUAL
    {'\b', '\b'}, // KEYCODE_BACKSPACE
    {'\t', '\t'}, // KEYCODE_TAB
    {'q', 'Q'},   // KEYCODE_Q
    {'w', 'W'},   // KEYCODE_W
    {'e', 'E'},   // KEYCODE_E
    {'r', 'R'},   // KEYCODE_R
    {'t', 'T'},   // KEYCODE_T
    {'y', 'Y'},   // KEYCODE_Y
    {'u', 'U'},   // KEYCODE_U
    {'i', 'I'},   // KEYCODE_I
    {'o', 'O'},   // KEYCODE_O
    {'p', 'P'},   // KEYCODE_P
    {'[', '{'},   // KEYCODE_LBRACKET
    {']', '}'},   // KEYCODE_RBRACKET
    {'\n', '\n'}, // KEYCODE_ENTER
    {0, 0},       // KEYCODE_LCONTROL
    {'a', 'A'},   // KEYCODE_A
    {'s', 'S'},   // KEYCODE_S
    {'d', 'D'},   // KEYCODE_D
    {'f', 'F'},   // KEYCODE_F
    {'g', 'G'},   // KEYCODE_G
    {'h', 'H'},   // KEYCODE_H
    {'j', 'J'},   // KEYCODE_J
    {'k', 'K'},   // KEYCODE_K
    {'l', 'L'},   // KEYCODE_L
    {';', ':'},   // KEYCODE_SEMICOLON
    {'\'', '"'},  // KEYCODE_SQUOTE
    {'`', '~'},   // KEYCODE_BACKTICK
    {0, 0},       // KEYCODE_LSHIFT
    {'\\', '|'},  // KEYCODE_BACKSLASH
    {'z', 'Z'},   // KEYCODE_Z
    {'x', 'X'},   // KEYCODE_X
    {'c', 'C'},   // KEYCODE_C
    {'v', 'V'},   // KEYCODE_V
    {'b', 'B'},   // KEYCODE_B
    {'n', 'N'},   // KEYCODE_N
    {'m', 'M'},   // KEYCODE_M
    {',', '<'},   // KEYCODE_COMMA
    {'.', '>'},   // KEYCODE_PERIOD
    {'/', '?'},   // KEYCODE_SLASH
    {0, 0},       // KEYCODE_RSHIFT
    {'*', '*'},   // KEYCODE_KPASTERISK
    {0, 0},       // KEYCODE_LALT
    {' ', ' '},   // KEYCODE_SPACE
    {0, 0},       // KEYCODE_CAPS
    {0, 0},       // KEYCODE_F1
    {0, 0},       // KEYCODE_F2
    {0, 0},       // KEYCODE_F3
    {0, 0},       // KEYCODE_F4
    {0, 0},       // KEYCODE_F5
    {0, 0},       // KEYCODE_F6
    {0, 0},       // KEYCODE_F7
    {0, 0},       // KEYCODE_F8
    {0, 0},       // KEYCODE_F9
    {0, 0},       // KEYCODE_F10
    {0, 0},       // KEYCODE_NUMLOCK
    {0, 0},       // KEYCODE_SCRLOCK
    {'7', '7'},   // KEYCODE_KP7
    {'8', '8'},   // KEYCODE_KP8
    {'9', '9'},   // KEYCODE_KP9
    {'-', '-'},   // KEYCODE_KPMINUS
    {'4', '4'},   // KEYCODE_KP4
    {'5', '5'},   // KEYCODE_KP5
    {'6', '6'},   // KEYCODE_KP6
    {'+', '+'},   // KEYCODE_KPPLUS
    {'1', '1'},   // KEYCODE_KP1
    {'2', '2'},   // KEYCODE_KP2
    {'3', '3'},   // KEYCODE_KP3
    {'0', '0'},   // KEYCODE_KP0
    {'.', '.'},   // KEYCODE_KPPERIOD
    {0, 0},       // 0x54
    {0, 0},       // 0x55
    {0, 0},       // 0x56
    {0, 0},       // KEYCODE_F11
    {0, 0},       // KEYCODE_F12
};

static volatile char charbuf;
static volatile bool char_available = FALSE;
static volatile bool char_requested = FALSE;

static __attribute__((interrupt)) void keyboard_handler(void *irq_frame)
{
    (void)irq_frame;
    /* Lowest bit of status will be set if buffer is not empty */
    uint8_t      scancode = ps2_get_scancode();
    bool         release  = (scancode & 0x80) != 0;
    enum Keycode key      = (enum Keycode)(scancode & 0x7f);
    if (key == KEYCODE_LSHIFT) {
        lshift_active = !release;
    } else if (key == KEYCODE_RSHIFT) {
        rshift_active = !release;
    } else if (key == KEYCODE_LCONTROL) {
        lcontrol_active = !release;
    } else if (key == KEYCODE_LALT) {
        lalt_active = !release;
    }

    char c = keymap[scancode][lshift_active || rshift_active];
    if (char_requested && !release && c != 0) {
        charbuf        = c;
        char_available = TRUE;
    }

    /* kaos_printf("got key: %b (%s)\n", key, release?"released":"pressed"); */

    /* switch(scancode) { */
    /* case 0x02: // 0 pressed */
    /* case 0x03: // 0 pressed */
    /* case 0x04: // 0 pressed */
    /* case 0x05: // 0 pressed */
    /* case 0x06: // 0 pressed */
    /* case 0x06: // 0 pressed */
    /* case 0x06: // 0 pressed */
    /* case 0x06: // 0 pressed */
    /* case 0x06: // 0 pressed */
    /* case 0x0b: // 0 pressed */
    /* case 0x2a: // left shift pressed */
    /*     lshift_active = TRUE; */
    /*     break; */
    /* case 0xaa: // left shift release */
    /*     lshift_active = FALSE; */
    /*     break; */
    /* case 0x36: // right shift pressed */
    /*     rshift_active = TRUE; */
    /*     break; */
    /* case 0xb6: // right shift release */
    /*     rshift_active = FALSE; */
    /*     break; */
    /* } */
    // translate scancode
    pic_eoi(PIC_IRQ_KEYBOARD);
}

void keyboard_init()
{
    int res = ps2_conf_read();
    if (res < 0) {
        KAOS_PANIC("Failed to read PS2 config");
    }
    uint8_t configuration = (uint8_t)res;
    kaos_printf("[ps2] configuration: %b\n", configuration);

    pic_enable_interrupt(PIC_IRQ_KEYBOARD);
    irq_register_handler(IRQ_VEC_KEYBOARD, keyboard_handler, 0x8E);
}

char keyboard_getchar()
{
    // either the buffer runs out, or we get an end of line
    char_available = FALSE;
    char_requested = TRUE;
    while (!char_available) {
        asm("hlt");
    }

    char_requested = FALSE;
    return charbuf;
}
