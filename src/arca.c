#include <dos.h>
#include <stdio.h>

#include "shared.h"

#define tr_base (outpsp + 0x10)

#define seg003  (tr_base + 0x00d8)
#define seg012  (tr_base + 0x0692)
#define seg013  (tr_base + 0x06b3)
#define seg015  (tr_base + 0x071d)
#define dseg    (tr_base + 0x0c2d)

/* last parameter pushed is last in arg list */

typedef void (far pascal *exit_with_error_t) (int e, int error_type, char far* message);
typedef int  (far pascal *get_line_from_pti_internal_t)(int idx, char far* dst, void far* pti_object);

exit_with_error_t exit_with_error;
get_line_from_pti_internal_t get_line_from_pti_internal;

void far pascal check_if_file_not_found(char far * file) {
    FILE *test;
    char *cfilename;

    ds_trampoline_start();

    cfilename = pascal_to_c(file);
    test = fopen(cfilename, "r");

    if (!test) {
        exit_with_error_t e = exit_with_error;

        ds_trampoline_end();
        e(5, 3, file);
    }
    else {
        fclose(test);
        ds_trampoline_end();
    }
}

void far pascal get_line_from_pti(int idx) {
    char far* out_pstring;
    void far* far* pti;
    char line[0x100];

    ds_trampoline_start();

    pti = MK_FP(dseg, 0x4c72);

    {
        get_line_from_pti_internal_t g;

        g = get_line_from_pti_internal;
        ds_trampoline_end();
        g(idx, line, *pti);
    }

    out_pstring = *(char far **)((char _ss *)&idx + 2);
    copy_c_to_pascal(line, out_pstring);

    {
        char size = *out_pstring;

        out_pstring[size] = ' ';
        format_word(out_pstring + size + 1, idx);
        *out_pstring = size + 5;
    }
}

char far* sucap = "\x04suca";

void far pascal configure_menu() {
    ds_trampoline_start();

    set_text_mode();
    dump_c_string("porcoddio");
    printf("madonna maiala\n");
    getch();

    asm mov ax, 0x13;
    asm int 10h;
    ds_trampoline_end();
}

void main() {
    int i;
    char far *str;

    unsigned long far* calltgt;
 
    ds_trampoline_init();

    if (load_program("c:\\tr\\manager.exe")) {
        printf("cannot load manager.exe\n");
        return;
    }

    /* patch to intercept */
    calltgt = MK_FP(seg003, 0x0672);
    *calltgt = (unsigned long) &check_if_file_not_found;

    /* get the original function */
    exit_with_error = MK_FP(seg015, 0x0310);
    get_line_from_pti_internal = MK_FP(seg013, 0x02c8);

    /* completely replace get_line_from_pti */
    patch_far_jmp(MK_FP(seg012, 0x001c), &get_line_from_pti);

    patch_far_jmp(MK_FP(seg003, 0x1b01), &configure_menu);

    start_program();
}

