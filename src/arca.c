#include <dos.h>
#include <stdio.h>

#include "shared.h"


#define arca_base (outpsp + 0x10)

#define seg006 (arca_base + 0x046f)
#define seg007 (arca_base + 0x0592)
#define seg008 (arca_base + 0x05a7)
#define seg012 (arca_base + 0x08fa)
#define seg013 (arca_base + 0x0961)
#define seg015 (arca_base + 0x0a2d)
#define dseg   (arca_base + 0x0eb9)


unsigned int far* background_buffer;
unsigned char far* highlight_frame_nr;
void far* far* far* status_ele_block;
unsigned int far* far* logi_tab_file;
void far* far* far* mat_filenames;
int far* gfx_bobs_color_override;
void far* far* pti_file_content;

void far* far* bobs_ele_item;
int  far* bobs_sizeof;
char far* bobs_palette_start;
char far* bobs_flip;
int  far* bobs_color;
int  far* bobs_x;
int  far* bobs_y;
int  far* bobs_count;


/* last parameter pushed is last in arg list */
typedef void (far pascal *render_ele_t) (int x, int y, void far* ele, int boh);
typedef void (far pascal *wait_vsync_t) (void);
typedef void (far pascal *get_tile_type_for_x_y_t) (int x, int y, char far* out);
typedef void (far pascal *render_string_t) (int x, int y, char far* string, int color);
typedef char (far pascal *logi_tab_contains_t) (int thing, int log_tab_index);
typedef void (far pascal *render_background_layer_t) (int mat_index);
typedef int  (far pascal *get_line_from_pti_internal_t)(int idx, char far* dst, void far* pti_object);
typedef void (far pascal *gfx_1_t)(int boh1, int boh2, int boh3, int boh4);
typedef void (far pascal *gfx_2_t)(int boh1, int boh2, int boh3, int boh4, char boh5);
typedef void (far pascal *gfx_3_t)(int boh1, int boh2, int boh3, int boh4, char boh5);
typedef int (far pascal *get_text_width_t)(char far* str);

render_ele_t render_ele;
render_ele_t render_ele_flipped;
wait_vsync_t wait_vsync_theirs;
get_tile_type_for_x_y_t get_tile_type_for_x_y;
render_string_t render_string;
logi_tab_contains_t logi_tab_contains_theirs;
render_background_layer_t render_background_layer;
get_line_from_pti_internal_t get_line_from_pti_internal;
gfx_1_t gfx_1;
gfx_2_t gfx_2;
gfx_3_t gfx_3;
get_text_width_t get_text_width;

int far pascal logi_tab_contains(int thing, int logi_tab_index);


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

void far pascal vga_dump(int x, int y, char far* s) {
    unsigned int old_buffer;
    char string[0x100];

    old_buffer = *background_buffer;
    *background_buffer = 0xa000;

    copy_c_to_pascal(s, string);

    {
        render_string_t r = render_string;

        ds_trampoline_end();
        r(x, y, string, 0x17);
        ds_trampoline_start();
    }

    *background_buffer = old_buffer;
}

void far pascal draw_highlight_under_cursor() {
    unsigned int old_buffer;
    int x, y;

    ds_trampoline_start();

    old_buffer = *background_buffer;
    *background_buffer = 0xa000;

    for (y = 0; y < 0x13; y++) {
        for (x = 1; x < 0x12; x++) {
            int pixel_x = x * 16;
            int pixel_y = y * 10;

            char data1;
            char data2;

            char top_25;
            char top_28;
            char bottom_25;

            {
                get_tile_type_for_x_y_t get_type = get_tile_type_for_x_y;

                ds_trampoline_end();
                get_type(pixel_x + 8, pixel_y,      &data1);
                ds_trampoline_start();

                ds_trampoline_end();
                get_type(pixel_x + 8, pixel_y + 10, &data2);
                ds_trampoline_start();
            }
            
            top_25 = logi_tab_contains(data1, 0x25);
            top_28 = logi_tab_contains(data1, 0x28);
            bottom_25 = logi_tab_contains(data2, 0x25);

            if ((top_25 || top_28) && !bottom_25) 
            {
                render_ele_t e = render_ele;
                render_string_t s = render_string;

                void far *ele = (*status_ele_block)[4 + *highlight_frame_nr];
                char far* thing = " 111";

                thing[0] = 3;
                thing[1] = top_25 ? '1' : '.';
                thing[2] = top_28 ? '1' : '.';
                thing[3] = bottom_25 ? '1' : '.';

                ds_trampoline_end();
                e(pixel_x, pixel_y, ele, -16);
                ds_trampoline_start();

                ds_trampoline_end();
                s(pixel_x, pixel_y, thing, 0x17);
                ds_trampoline_start();
            }
        }
    }

    vga_dump(0, 0, "diocane");

    wait_vsync();
    wait_vsync();
    wait_vsync();

    *highlight_frame_nr = (*highlight_frame_nr + 1) % 3;
    *background_buffer = old_buffer;

    ds_trampoline_end();
}

int far pascal logi_tab_contains(int thing, int logi_tab_index) {
    unsigned int offset;
    unsigned char far* data;
    char retval = 0;

    thing &= 0xff;
    logi_tab_index &= 0xff;

    offset = (*logi_tab_file)[logi_tab_index];
    offset = ((offset & 0x00ff) << 8) | ((offset & 0xff00) >> 8);
    data = (unsigned char far *)(*logi_tab_file) + offset;

    while (*data != 0xff) {
        if (*data == thing) {
            retval = 1;
            break;
        }

        data++;
    }

    return retval;
}

int far pascal logi_tab_contains_w(int thing, int logi_tab_index) {
    int x;

    ds_trampoline_start();
    x = logi_tab_contains(thing, logi_tab_index);
    ds_trampoline_end();

    return x;
}

void far pascal render_all_background_layers() {
    int i;
    render_background_layer_t r;

    ds_trampoline_start();

    r = render_background_layer;

    for (i = 0; i < 4; i++) {
        int mat = i == 3 ? 9 : i;

        if ((*mat_filenames)[mat] != 0) {
            ds_trampoline_end();
            r(mat);
            ds_trampoline_start();
        }
    }

    ds_trampoline_end();
}

void far pascal add_bob_per_background(
    int x,
    int y,
    void far* ele_item,
    int palette_start,
    int flip,
    int color
) {
    int i;

    ds_trampoline_start();

    i = *bobs_count;
    *bobs_count = i + 1;

    if (*bobs_count > 0x32) {
        /* bobs overflow error */
        ds_trampoline_end();
        return;
    }

    bobs_x[i] = x;
    bobs_y[i] = y;
    bobs_ele_item[i] = ele_item;
    bobs_palette_start[i] = palette_start;
    bobs_flip[i] = flip;
    bobs_color[i] = color;

    ds_trampoline_end();
}

char far pascal bobs_get_count() {
    char ret;

    ds_trampoline_start();
    ret = *bobs_count;
    ds_trampoline_end();
    return ret;
}

void far pascal render_bobs_in_background() {
    char count;
    char i;

    ds_trampoline_start();

    count = *bobs_count;

    for (i = 0; i < count; i++) {

        if (!bobs_sizeof[i])
            continue;

        if (bobs_color[i])
            *gfx_bobs_color_override = bobs_color[i];

        {
            render_ele_t e = bobs_flip[i] ? render_ele_flipped : render_ele;

            int x = bobs_x[i];
            int y = bobs_y[i];
            void far* ele = bobs_ele_item[i];
            int pal = bobs_palette_start[i];

            ds_trampoline_end();
            e(x, y, ele, pal);
            ds_trampoline_start();
        }

        if (bobs_color[i])
            *gfx_bobs_color_override = 0xc7;
    }

    ds_trampoline_end();
}

void get_line_from_pti_c(int idx, char far* out_pstring) {
    char line[0x100];

    ds_trampoline_start();

    {
        get_line_from_pti_internal_t g;
        void far* far* pti = pti_file_content;

        g = get_line_from_pti_internal;
        ds_trampoline_end();
        g(idx, line, *pti);
    }

    copy_c_to_pascal(line, out_pstring);


#if 0
    {
        char size = *out_pstring;

        out_pstring[size] = ' ';
        format_word(out_pstring + size + 1, idx);
        *out_pstring = size + 5;
    }
#endif
}

void far pascal get_line_from_pti(int idx) {
    char far* out_pstring;
    out_pstring = *(char far **)((char _ss *)&idx + 2);
    get_line_from_pti_c(idx, out_pstring);
}

void far pascal render_explanation_strings(
    char far* line1,
    char far* line2,
    char color,
    char boh2
) {
    get_text_width_t gtw;
    render_string_t rs;
    gfx_2_t g2;
    gfx_3_t g3;

    unsigned int old_buffer;

    int line1_width;
    int line2_width;

    int left;
    int right;

    ds_trampoline_start();

    gtw = get_text_width;
    g2 = gfx_2;
    g3 = gfx_3;
    rs = render_string;

    old_buffer = *background_buffer;
    *background_buffer = 0xa000;

    ds_trampoline_end();
    line1_width = gtw(line1);
    ds_trampoline_start();

    ds_trampoline_end();
    line2_width = gtw(line2);
    ds_trampoline_start();

    if (line1_width > line2_width) {
        left = (0x140 - line1_width) / 2;
        right = left + line1_width;
    }
    else {
        left = (0x140 - line2_width) / 2;
        right = left + line2_width;
    }
    
    if (left <      4) left  =     4;
    if (right > 0x13b) right = 0x13b;

    if (!line2 || (line2[0] == 1 && line2[1] == ' ')) {
        ds_trampoline_end();
        g2(left - 4, 0x55, right + 3, 0x67, boh2);
        ds_trampoline_start();

        ds_trampoline_end();
        g3(left - 3, 0x56, right + 2, 0x66, color);
        ds_trampoline_start();
    }
    else {
        ds_trampoline_end();
        g2(left - 4, 0x55, right + 3, 0x70, boh2);
        ds_trampoline_start();

        ds_trampoline_end();
        g3(left - 3, 0x56, right + 2, 0x6f, color);
        ds_trampoline_start();
    }

    ds_trampoline_end();
    rs((0x140 - line1_width) / 2, 0x5a, line1, color);
    ds_trampoline_start();

    if (line2[0] != 1 && line2[1] != ' ') {
        ds_trampoline_end();
        rs((0x140 - line2_width) / 2, 0x64, line2, color);
        ds_trampoline_start();
    }

    *background_buffer = old_buffer;
    ds_trampoline_end();
}

void far pascal render_context_explanation(int line1_id, int line2_id) {
    char line1[0x50];
    char line2[0x50];

    get_line_from_pti_c(line1_id, line1);
    get_line_from_pti_c(line2_id, line2);

    render_explanation_strings(line1, line2, 0xffcf, 0);
}

void far pascal render_help_string(int far* y, char far* string, char color) {
    render_string_t rs;
    unsigned int old_buffer;

    ds_trampoline_start();

    rs = render_string;

    old_buffer = *background_buffer;
    *background_buffer = 0xa000;

    ds_trampoline_end();
    rs(4, *y, string, color);
    ds_trampoline_start();

    *y += 10;

    *background_buffer = old_buffer;
    ds_trampoline_end();
}

void far pascal render_key_help(int color, int boh) {
    gfx_1_t g1;
    gfx_2_t g2;
    gfx_3_t g3;
    render_string_t rs;

    unsigned int old_buffer;
    char line[0x100];
    int text_y;
    int i;

    ds_trampoline_start();

    g1 = gfx_1;
    g2 = gfx_2;
    g3 = gfx_3;
    rs = render_string;

    old_buffer = *background_buffer;
    *background_buffer = 0xa000;

    ds_trampoline_end();
    g1(0, 0, 0x13f, 0xc7);
    g2(0, 0, 0x13f, 0xc7, boh);
    g3(0, 0, 0x13f, 0xc7, color);
    ds_trampoline_start();

    text_y = 4;

    for (i = 0; i < 0x13; i++) {
        ds_trampoline_end();
        get_line_from_pti_c(0x88b8 + i, line);
        ds_trampoline_start();

        ds_trampoline_end();
        rs(4, text_y, line, color);
        ds_trampoline_start();

        text_y += 10;
    }

    *background_buffer = old_buffer;
    ds_trampoline_end();
}

void init_pointers() {
    highlight_frame_nr       = MK_FP(dseg, 0x0100);
    pti_file_content         = MK_FP(dseg, 0x2cbc);
    status_ele_block         = MK_FP(dseg, 0x2f0c);
    mat_filenames            = MK_FP(dseg, 0x2f58);
    logi_tab_file            = MK_FP(dseg, 0x2f14);
    bobs_ele_item            = MK_FP(dseg, 0x383c);
    bobs_sizeof              = MK_FP(dseg, 0x3904);
    bobs_palette_start       = MK_FP(dseg, 0x3968);
    bobs_flip                = MK_FP(dseg, 0x399a);
    bobs_color               = MK_FP(dseg, 0x39cc);
    bobs_x                   = MK_FP(dseg, 0x3af8);
    bobs_y                   = MK_FP(dseg, 0x3b5c);
    bobs_count               = MK_FP(dseg, 0x3bc0);
    background_buffer        = MK_FP(dseg, 0x3d20);
    gfx_bobs_color_override  = MK_FP(dseg, 0x3d28);

    get_line_from_pti_internal = MK_FP(seg008, 0x02c8);
    logi_tab_contains_theirs   = MK_FP(seg012, 0x0603);
    render_background_layer    = MK_FP(seg013, 0x0244);
    get_tile_type_for_x_y      = MK_FP(seg013, 0x061e);
    get_text_width             = MK_FP(seg015, 0x04fc);
    render_string              = MK_FP(seg015, 0x05f9);
    wait_vsync_theirs          = MK_FP(seg015, 0x1311);
    gfx_1                      = MK_FP(seg015, 0x136b);
    gfx_3                      = MK_FP(seg015, 0x1511);
    gfx_2                      = MK_FP(seg015, 0x1595);
    render_ele                 = MK_FP(seg015, 0x1b8e);
    render_ele_flipped         = MK_FP(seg015, 0x1bc3);


    patch_far_jmp(MK_FP(seg006, 0x10bb), &draw_highlight_under_cursor);
    patch_far_jmp(MK_FP(seg006, 0x0fcb), &render_context_explanation);
    patch_far_jmp(MK_FP(seg006, 0x101c), &render_key_help);
    patch_far_jmp(MK_FP(seg007, 0x001d), &get_line_from_pti);
    patch_far_jmp(MK_FP(seg012, 0x0603), &logi_tab_contains_w);
    patch_far_jmp(MK_FP(seg013, 0x048d), &render_all_background_layers);
    patch_far_jmp(MK_FP(seg013, 0x070a), &bobs_get_count);
    patch_far_jmp(MK_FP(seg013, 0x071d), &add_bob_per_background);
    patch_far_jmp(MK_FP(seg013, 0x0b64), &render_bobs_in_background);
    patch_far_jmp(MK_FP(seg015, 0x0eca), &render_help_string);
}

void main(int argc, char *argv[]) {
    ds_trampoline_init();

    set_text_mode();
    printf("argc %d\narg1 %s\n", argc, argv[1]);
    printf("my  psp: %x\n", getpsp());

    setup_cmdline(argv[1]);

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

