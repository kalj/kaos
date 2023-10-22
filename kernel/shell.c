#include "shell.h"

#include "kaos.h"
#include "kaos_bool.h"
#include "keyboard.h"
#include "max_min.h"

static int get_line(char *buf, int bufline)
{
    int pos = 0;
    while (TRUE) {
        char c = keyboard_getchar();
        kaos_putc(c);

        if (c == '\n') {
            // write trailing 0 byte
            buf[pos] = '\0';
            return pos;
        } else {
            // echo to wherever
            if (c == '\b') {
                pos = MAX(pos - 1, 0);
            } else if (pos <
                       (bufline - 1)) { // we are pointing left of the last valid slot (which is dedicated to a 0 byte
                buf[pos] = c;
                pos++;
            } else {
                return -1;
            }
        }
    }
}

/* int find_char_pos(const char *buf, int buflen, char c) { */
/*     for(int i=0; i<buflen && buf[i]; i++) { */
/*         if(buf[i] == c) { */
/*             return i; */
/*         } */
/*     } */
/*     return -1; */
/* } */

static char      line_buf[120];
const static int buf_size = sizeof(line_buf);

void shell_run()
{
    while (TRUE) {

        kaos_puts("$ ");
        int len = get_line(line_buf, buf_size);
        if (len < 0) {
            kaos_printf("Too long line!\n");
            continue;
        }

        /* kaos_printf("Line to process: %s\n", line_buf); */

        int pos = 0;
        // skip spaces
        for (; pos < len && (line_buf[pos] == ' ' || line_buf[pos] == '\t'); pos++) {
        }

        // if nothing left, simply skip
        if (pos == len) continue;

        // print command
        kaos_puts("Got command: ");
        for (; line_buf[pos] != ' ' && line_buf[pos] != '\t' && pos < len; pos++) {
            kaos_putc(line_buf[pos]);
        }

        kaos_putc('\n');

        // skip spaces
        for (; pos < len && (line_buf[pos] == ' ' || line_buf[pos] == '\t'); pos++) {
        }

        // if nothing left, simply skip
        if (pos == len) continue;

        kaos_puts("       args: ");
        for (; pos < len; pos++) {
            kaos_putc(line_buf[pos]);
        }
        kaos_putc('\n');

        /* int space_pos = find_char_pos(line_buf, sizeof(line_buf), ' '); */
        /* if(space_pos > 0) { */

        /* } */
        /* if(strequal(word, "quit")) { */
        /*     // break while loop and end */
        /* } else if(strequal(word, "echo")) { */
        /*     // print any other args  */
        /* } else if(strequal(word, "print_memory_map")) { */
        /*     // run print_memory_map() */
        /* } else if(strequal(word, "time")) { */
        /*     // print time */
        /* } else { */
        /*     // unknown: panic */
        /* } */
    }
}
