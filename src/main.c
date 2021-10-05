#include <dos.h>
#include <stdio.h>

extern int outpsp;
extern void far * csip;
extern int myds;

extern int load_program(char near *cosa);
extern void start_program();

extern int t1(int x);
extern int t2();

extern void ds_trampoline_init();
extern void ds_trampoline_start();
extern void ds_trampoline_end();

/*
    new psp: 12b9 (example... those change)
    cs ip:   12c9:016e (seg000:PROGRAM)

    ida relocates at 1000:0000
    ida2dosbox: (ida - 1000) + (newpsp + 0010)
                (ida - 1000) + 12c9
*/

#define tr_base (outpsp + 0x10)

#define seg003  (tr_base + 0x00d8)
#define seg015  (tr_base + 0x071d)
#define dseg    (tr_base + 0x0c2d)

void dump_pascal_string(char far *string) {
    char size = *string;
    char i;

    for (i = 0; i < size; i++) {
        char c = *(string + 1 + i);

        asm mov ah, 0x02;
        asm mov dl, c;
        asm int 21h;
    }
}


void dump_c_string(char *string) {
    char *s = string;
    char c;

    do {
        c = *s++;

        asm mov ah, 0x02;
        asm mov dl, c;
        asm int 21h;
    } while(c);
}

char *pascal_to_c(char far *string) {
    static char cstring[0x100];

    char i, count;

    count = *string;
    cstring[count] = 0;

    for (i = 0; i < count; i++)
        cstring[i] = string[1 + i];

    return cstring;
}

void set_text_mode() {
    asm mov ax, 0x03;
    asm int 10h;
}

typedef (far pascal *exit_with_error_t)(int, int, char far *);

exit_with_error_t exit_with_error;

void far pascal check_if_file_not_found(char far * file) {
    FILE *test;
    char *cfilename;

    ds_trampoline_start();

    cfilename = pascal_to_c(file);

/*
    set_text_mode();
    dump_pascal_string(file);
    dump_c_string(cfilename);
*/

    test = fopen(cfilename, "r");

    if (!test) {
        exit_with_error_t e = exit_with_error;

        dump_c_string("die");

        ds_trampoline_end();
        e(5, 3, file);
    }
    else {
        fclose(test);
        ds_trampoline_end();
    }
}

void main() {
    int i;
    char far *str;

    unsigned long far* calltgt;
 
    void far *func = &check_if_file_not_found;

    struct SREGS segreg;

    segread(&segreg);
    ds_trampoline_init();

    printf("segreg cs %x\n", segreg.cs);
    printf("segreg ds %x\n", segreg.ds);
    printf("t1 %x\n", t2());


    if (load_program("c:\\tr\\manager.exe")) {
        printf("cannot load manager.exe\n");
        return;
    }

/*
    printf("long:    %d\n", sizeof(long));
    printf("new psp: %4x\n", outpsp);
    printf("cs:ip:   %8lx\n", csip);
    printf("func     %8lx\n", func);

    str = MK_FP(dseg, 0x1c3);

    do {
        putchar(*str);
        str++;
    } while (*str != 0);

    putchar('\n');
*/

    /* patch to intercept */
    calltgt = MK_FP(seg003, 0x0672);
    *calltgt = (unsigned long) func;

    /* get the original function */
    exit_with_error = MK_FP(seg015, 0x0310);
    

    start_program();
}

