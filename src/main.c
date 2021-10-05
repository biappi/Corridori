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
#define seg012  (tr_base + 0x0692)
#define seg013  (tr_base + 0x06b3)
#define seg015  (tr_base + 0x071d)
#define dseg    (tr_base + 0x0c2d)


void set_text_mode() {
    asm mov ax, 0x03;
    asm int 10h;
}

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

void dump_c_string(char far *string) {
    char far *s = string;
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

void copy_c_to_pascal(char far * cstring, char far * out) {
    char c;
    char i = 0;

    do {
        c = cstring[i];
        out[i + 1] = c;
        i++;
    } while (c != 0);

    *out = i;
}

char nibble_to_char(char n) {
    n = n & 0x0f;
    return n > 10 ? n - 10 + 'a' : n + '0';
}

void format_byte(char far *dst, char byte) {
    dst[0] = nibble_to_char((byte & 0xf0) >> 4);
    dst[1] = nibble_to_char((byte & 0x0f) >> 0);
}

void format_word(char far *dst, int word) {
    format_byte(dst + 0, (word & 0xff00) >> 8);
    format_byte(dst + 2, (word & 0x00ff) >> 0);
}

void format_ptr(char far *dst, void far * ptr) {
    format_word(dst + 0, FP_SEG(ptr));
    format_word(dst + 5, FP_OFF(ptr));
    dst[4] = ':';
}

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

void patch_far_jmp(void far * addr, void far * dst) {
    unsigned char far* original = addr;
    unsigned long func = (unsigned long) dst;

    *original++ = 0xea; /* JMP ptr16:16 */
    *original++ = (func >>  0) & 0xff;
    *original++ = (func >>  8) & 0xff;
    *original++ = (func >> 16) & 0xff;
    *original++ = (func >> 24) & 0xff;
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
    *calltgt = (unsigned long) &check_if_file_not_found;

    /* get the original function */
    exit_with_error = MK_FP(seg015, 0x0310);
    get_line_from_pti_internal = MK_FP(seg013, 0x02c8);

    /* completely replace get_line_from_pti */
    patch_far_jmp(MK_FP(seg012, 0x001c), &get_line_from_pti);

    patch_far_jmp(MK_FP(seg003, 0x1b01), &configure_menu);

    start_program();
}

