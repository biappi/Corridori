#include <dos.h>
#include <stdio.h>

#include "shared.h"


#define arca_base (outpsp + 0x10)

#define seg006 (arca_base + 0x046f)
#define seg013 (arca_base + 0x0961)
#define seg015 (arca_base + 0x0a2d)
#define dseg   (arca_base + 0x0eb9)


unsigned int far* background_buffer; /* segment! */
unsigned char far* highlight_frame_nr;
void far* far* far* status_ele_block;

/* last parameter pushed is last in arg list */
typedef void (far pascal *render_ele_t) (int x, int y, void far* ele, int boh);
typedef void (far pascal *wait_vsync_t) (void);
typedef void (far pascal *get_tile_type_for_x_y_t) (int x, int y, void far* out);
typedef void (far pascal *render_string_t) (int x, int y, char far* string, int color);

render_ele_t render_ele;
wait_vsync_t wait_vsync_theirs;
get_tile_type_for_x_y_t get_tile_type_for_x_y;
render_string_t render_string;

void wait_vsync() {
    asm mov dx, 0x03da;

loop1:
    asm in al, dx
    asm and al, 8
    asm jnz loop1

loop2:
    asm in al, dx
    asm and al, 8
    asm jz loop2
}

void far pascal test_vga() {
    int x, y;
    char c = 0x80;
    char far* screen = MK_FP(0xa000, 0x0000);

    ds_trampoline_start();

    for (x = 0; x < 50; x++) {
        for (y = 0; y < 50; y++) {
            screen[320 * y + x] = c++;
        }
    }

    for (x = 0; x < 5; x++)
        wait_vsync();

    ds_trampoline_end();
}

void far pascal draw_highlight_under_cursor() {
    static unsigned int saved_si;
    static unsigned int saved_di;

    int x, y;

    unsigned int old_buffer;

    ds_trampoline_start();

    old_buffer = *background_buffer;

    *background_buffer = 0xa000;


    for (y = 0; y < 0x13; y++) {
        for (x = 1; x < 0x12; x++) {
            int pixel_x = x * 16;
            int pixel_y = y * 10;

            {
                render_ele_t e = render_ele;
                void far *ele = (*status_ele_block)[4 + *highlight_frame_nr];
                ds_trampoline_end();
                e(pixel_x, pixel_y, ele, -16);
                ds_trampoline_start();
            }
        }
    }

    {
        render_string_t r = render_string;
        char far* diocane = " diocane";

        *diocane = 7;

        ds_trampoline_end();
        r(0, 0, diocane, 0x17);
        ds_trampoline_start();
    }

    wait_vsync();
    wait_vsync();
    wait_vsync();

    *highlight_frame_nr = (*highlight_frame_nr + 1) % 3;
    *background_buffer = old_buffer;

    ds_trampoline_end();
}

void init_pointers() {
    background_buffer = MK_FP(dseg, 0x3d20);
    status_ele_block = MK_FP(dseg, 0x2f0c);
    highlight_frame_nr = MK_FP(dseg, 0x0100);

    get_tile_type_for_x_y = MK_FP(seg013, 0x061e);
    render_string         = MK_FP(seg015, 0x05f9);
    wait_vsync_theirs     = MK_FP(seg015, 0x1311);
    render_ele            = MK_FP(seg015, 0x1b8e);

    patch_far_jmp(MK_FP(seg006, 0x10bb), &draw_highlight_under_cursor);
}

void main(int argc, char *argv[]) {
    ds_trampoline_init();

    set_text_mode();
    printf("argc %d\narg1 %s\n", argc, argv[1]);
    printf("my  psp: %x\n", getpsp());

    {
        extern char cmdline[0x100];

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

    set_vga_mode();

    init_pointers();
    start_program();
}

