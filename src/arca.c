#include <dos.h>
#include <stdio.h>

#include "shared.h"

#define arca_base (outpsp + 0x10)

/* last parameter pushed is last in arg list */

typedef void (far pascal *exit_with_error_t) (int e, int error_type, char far* message);
typedef int  (far pascal *get_line_from_pti_internal_t)(int idx, char far* dst, void far* pti_object);

exit_with_error_t exit_with_error;
get_line_from_pti_internal_t get_line_from_pti_internal;

extern char cmdline[0x100];

void main(int argc, char *argv[]) {
    ds_trampoline_init();

    set_text_mode();
    printf("argc %d\narg1 %s\n", argc, argv[1]);
    printf("my  psp: %x\n", getpsp());

    {
        char *c = argv[1];
        char *s = cmdline;
        char *l = cmdline + 1;

        *s = 1;

        *l++ = ' ';

        do {
            (*s)++;
            *l++ = *c++;
        } while (*c != 0);

        *l = 0x0d;
    }

    if (load_program("c:\\tr\\cods\\arca.exe")) {
        printf("cannot load arca.exe\n");
        return;
    }

    printf("new psp: %x\n", outpsp);
    printf("start...\n");
    getch();

    asm mov ax, 0x13;
    asm int 10h;

    start_program();
}

