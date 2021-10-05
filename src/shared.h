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

/* - */

void patch_far_jmp(void far * addr, void far * dst) {
    unsigned char far* original = addr;
    unsigned long func = (unsigned long) dst;

    *original++ = 0xea; /* JMP ptr16:16 */
    *original++ = (func >>  0) & 0xff;
    *original++ = (func >>  8) & 0xff;
    *original++ = (func >> 16) & 0xff;
    *original++ = (func >> 24) & 0xff;
}

/* - */

void set_text_mode() {
    asm mov ax, 0x03
    asm int 10h;
}

void set_vga_mode() {
    asm mov ax, 0x13;
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
    return n >= 10 ? n - 10 + 'a' : n + '0';
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

