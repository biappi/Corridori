#include <dos.h>
#include <stdio.h>

#include "shared.h"


#define arca_base (outpsp + 0x10)

#define seg002 (arca_base + 0x00f6)
#define seg003 (arca_base + 0x0325)
#define seg004 (arca_base + 0x0358)
#define seg005 (arca_base + 0x044f)
#define seg006 (arca_base + 0x046f)
#define seg007 (arca_base + 0x0592)
#define seg008 (arca_base + 0x05a7)
#define seg010 (arca_base + 0x06bb)
#define seg011 (arca_base + 0x06c8)
#define seg012 (arca_base + 0x08fa)
#define seg013 (arca_base + 0x0961)
#define seg015 (arca_base + 0x0a2d)
#define seg017 (arca_base + 0x0c8f)
#define seg021 (arca_base + 0x0dc0)
#define dseg   (arca_base + 0x0eb9)


unsigned int far* background_buffer;
unsigned char far* highlight_frame_nr;
void far* far* far* status_ele_block;
void far* far* far* tr_ele_file;
unsigned int far* far* logi_tab_file;
int far* gfx_bobs_color_override;
void far* far* pti_file_content;
char far* pupo_current_ani;
long far* far* swivar_block_1;
char far* far* swivar_block_2;
char far* far* swi_file_content;
void far* far* swi_file_elements;
void far* tiletype_actions;
int  far* animjoy_tab_file_seg;
int  far* animjoy_tab_file_off;
char far* far* animofs_tab_file;
int  far* frames_tab_file_seg;
int  far* frames_tab_file_off;
char far* get_new_ani;
char far* byte_1f4dc;
char far* byte_1f4e6;
char far* byte_1f4e8;
char far* byte_1f4e9;
int  far* pupo_offset;
void far* far* bobs_ele_item;
int  far* bobs_sizeof;
char far* bobs_palette_start;
char far* bobs_flip;
unsigned int far* bobs_color;
int  far* bobs_x;
int  far* bobs_y;
int  far* bobs_count;
unsigned int far* pu_file_buffer_seg;
unsigned int far* pu_file_buffer_off;
char far* far* far* ucci_image_content;
char far* enemy_flip;
char far* far* bobs_to_hit_mouse;

int  far* mouse_pointer_x;
int  far* mouse_pointer_y;
char far* mouse_inited;
char far* mouse_do_default_action;
void far* far* mouse_funcptr2;
int  far* mouse_pointer_file_size;
char far* far* mouse_pointer_file_buf;
char far* far* mouse_pointer_ele_1;
char far* far* mouse_pointer_ele_2;
char far* far* mouse_pointer_ele_3;
char far* bob_to_hit_mouse_3;
char far* up_pressed;
char far* down_pressed;
char far* left_pressed;
char far* right_pressed;
char far* l_shift_pressed;
char far* r_shift_pressed;
char far* esc_pressed_flag;

struct {
    char info;
    char file;
} far* mat_filenames;

int  far* tiles_ids;
char far* tiles_types;
int  far* tiles_overrides;
char far* current_mat_loaded;
char far* far* mat_buffer;
char far* background_ani_countdown;
char far* background_ani_frame;
char far* punti_countdown;
char far* faccia_countdown;
int  far* pupo_x;
int  far* pupo_y;
unsigned char far* pupo_tile_top;
unsigned char far* pupo_tile_bottom;
int  far* vita;
char far* colpi;
int  far* punti;
int  far* pupo_new_x;
int  far* pupo_new_y;
char far* gun_bool;
char far* pupo_current_frame;
char far* pupo_flip;
char far* pupo_x_delta;
char far* pupo_y_delta;
char far* pupo_anim_countdown;
int  far* to_set_pupo_x;
int  far* to_set_pupo_y;
char far* palette_mangling_counter;
char far* disable_pupo_anim;
int  far* pupo_palette_override;
char far* current_room_number;
char far* read_from_usc;
char far* no_previous_room;
char far* wanted_room;
char far* need_to_save_arc_lzr;
char far* maybe_exit_related;
char far* should_stop_gameloop;
char far* far* dsp_file;
char far* far* usc_file;
char far* far* prt_file;
char far* far* sostani_file;
char far* counter_caduta;



/* last parameter pushed is last in arg list */
typedef void (far pascal *render_ele_t) (int x, int y, void far* ele, int boh);
typedef void (far pascal *wait_vsync_t) (void);
typedef void (far pascal *render_string_t) (int x, int y, char far* string, int color);
typedef char (far pascal *logi_tab_contains_t) (int thing, int log_tab_index);
typedef void (far pascal *render_background_layer_t) (int mat_index);
typedef int  (far pascal *get_line_from_pti_internal_t)(int idx, char far* dst, void far* pti_object);
typedef void (far pascal *gfx_1_t)(int boh1, int boh2, int boh3, int boh4);
typedef void (far pascal *gfx_2_t)(int boh1, int boh2, int boh3, int boh4, char boh5);
typedef void (far pascal *gfx_3_t)(int boh1, int boh2, int boh3, int boh4, char boh5);
typedef int  (far pascal *get_text_width_t)(char far* str);
typedef int  (far pascal *mouse_button_status_t)(int far* button, int far* x, int far* y, int far* count);
typedef void (far pascal *mouse_click_event_t)(int x, int y, int button_id);
typedef void (far pascal *cammina_per_click_t)();
typedef void (far pascal *mouse_get_status_t)(int far* dx, int far* dy, int far* status);
typedef int  (far pascal *mouse_pointer_for_point_t)(int x, int y);
typedef void (far pascal *do_tiletype_actions_inner_t)(int boh);
typedef void (far pascal *tiletype_action_t)(void far* ptr, int far* idx, int far* top);
typedef void (far pascal *load_buffer_mat_t)(int x);
typedef void (far pascal *reset_clicked_button_t)();


render_ele_t render_ele;
render_ele_t render_ele_flipped;
wait_vsync_t wait_vsync_theirs;
render_string_t render_string;
logi_tab_contains_t logi_tab_contains_theirs;
render_background_layer_t render_background_layer;
get_line_from_pti_internal_t get_line_from_pti_internal;
gfx_1_t gfx_1;
gfx_2_t gfx_2;
gfx_3_t gfx_3;
get_text_width_t get_text_width;
mouse_button_status_t mouse_button_status;
mouse_click_event_t mouse_click_event;
cammina_per_click_t cammina_per_click;
mouse_get_status_t mouse_get_status;
mouse_pointer_for_point_t mouse_pointer_for_point;
do_tiletype_actions_inner_t do_tiletype_actions_inner_th;
load_buffer_mat_t load_buffer_mat;
reset_clicked_button_t reset_clicked_button;


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

void far pascal bg_dump(int x, int y, char far* s) {
    char string[0x100];
    copy_c_to_pascal(s, string);

    {
        render_string_t r = render_string;

        ds_trampoline_end();
        r(x, y, string, 0x17);
        ds_trampoline_start();
    }
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

void vga_dump_ptr(int far* y, char far* str, void far* ptr) {
    char *fmt = "xxxx:xxxx";
    format_ptr(fmt, ptr);
    vga_dump(0, *y, str);
    vga_dump(40, *y, fmt);
    (*y) += 8;
}

void vga_dump_byte(int far* y, char far* str, char b) {
    char *fmt = "xx";
    format_byte(fmt, b);
    vga_dump(0, *y, str);
    vga_dump(40, *y, fmt);
    (*y) += 8;
}

void vga_dump_word(int far* y, char far* str, int w) {
    char *fmt = "xxxx";
    format_word(fmt, w);
    vga_dump(0, *y, str);
    vga_dump(40, *y, fmt);
    (*y) += 8;
}

/* - */

#define TILES_WIDTH  16;
#define TILES_HEIGTH 10;

#define MAX(a, b) ((a) > (b) ? a : b)
#define MIN(a, b) ((a) < (b) ? a : b)

char far pascal set_is_member(char to_check, char far* set) {
    char bit  = 1 << (to_check & 7);
    char byte = set[to_check / 8];

    return !!(byte & bit);
}

char far pascal screen_to_tile_x(int screen_x) {
    int x = screen_x / TILES_WIDTH;
    return MIN(MAX(x, 0), 19);
}

char far pascal screen_to_tile_y(int screen_y) {
    int y = screen_y / TILES_HEIGTH;
    return MIN(MAX(y, 0), 20);
}


char far pascal get_tile_type(int x, int y) {
    int tx = screen_to_tile_x(x);
    int ty = screen_to_tile_y(y);

    return tiles_types[ty * 20 + tx];
}

void far pascal get_tile_type_w(int x, int y, char far* out) {
    ds_trampoline_start();
    *out = get_tile_type(x, y);
    ds_trampoline_end();
}

/* - */

void far pascal draw_highlight_under_cursor() {
    unsigned int old_buffer;
    int x, y;

    ds_trampoline_start();

    old_buffer = *background_buffer;
    *background_buffer = 0xa000;

    for (y = 0; y < 0x13; y++) {
        for (x = 1; x < 0x12; x++) {
            char top_25;
            char top_28;
            char bottom_25;

            int pixel_x = x * 16;
            int pixel_y = y * 10;

            char data1 = get_tile_type(pixel_x + 8, pixel_y     );
            char data2 = get_tile_type(pixel_x + 8, pixel_y + 10);

            top_25 = logi_tab_contains(data1, 0x25);
            top_28 = logi_tab_contains(data1, 0x28);
            bottom_25 = logi_tab_contains(data2, 0x25);

            if ((top_25 || top_28) && !bottom_25) 
            {
                render_ele_t e = render_ele;
                void far *ele = (*status_ele_block)[4 + *highlight_frame_nr];

                ds_trampoline_end();
                e(pixel_x, pixel_y, ele, -16);
                ds_trampoline_start();

                if (0) {
                    char far* thing = " 111";

                    thing[0] = 3;
                    thing[1] = top_25 ? '1' : '.';
                    thing[2] = top_28 ? '1' : '.';
                    thing[3] = bottom_25 ? '1' : '.';

                    vga_dump(pixel_x, pixel_y, thing);
                }
            }

            if (data1) {
                char *suca = "xx";
                format_byte(suca, data1);
                vga_dump(pixel_x, pixel_y, suca);
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

int from_big_endian(int x) {
    return x = ((x & 0x00ff) << 8) | ((x & 0xff00) >> 8);
}

int far pascal logi_tab_contains(int thing, int logi_tab_index) {
    unsigned int offset;
    unsigned char far* data;
    char retval = 0;

    thing &= 0xff;
    logi_tab_index &= 0xff;

    offset = (*logi_tab_file)[logi_tab_index];
    offset = from_big_endian(offset);
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

void far pascal render_background_layer_mine(int mat_idx) {
    int i;

    ds_trampoline_start();

    for (i = 0; i < 0x14 * 0x14; i++) {
        int original  = tiles_ids[i];
        int overrid   = tiles_overrides[i];
        int tile_info = from_big_endian(
            original == overrid ? 0xffff : original
        );

        char tile_mat = ((tile_info & 0xf000) >> 12);

        if (original == overrid)
            continue;

        if (tile_mat != mat_idx)
            continue;

        tiles_overrides[i] = original;

        if (*current_mat_loaded != mat_idx) 
        {
            load_buffer_mat_t lb = load_buffer_mat;
            char tileset = mat_filenames[mat_idx].file;

            ds_trampoline_end();
            lb(tileset);
            ds_trampoline_start();

            *current_mat_loaded = mat_idx;
        }

        {
            int tile_x = i % 0x14;
            int tile_y = i / 0x14;
            int tile_i = tile_info & 0x01ff;

            int flip   = !!(tile_info & 0x0200);

            static const tile_width = 0x10;
            static const tile_height = 0x0a;

            int tile_px_x = tile_x * tile_width;
            int tile_px_y = tile_y * tile_height;

            int stride = tile_width * 0x14;

            int src_offset = tile_i * (tile_width * tile_height);
            char far* dst = MK_FP(*background_buffer, 0);
            char far* src = *mat_buffer;

            int x, y;

            src += src_offset;

            for (y = 0; y < tile_height; y++) {
                for (x = 0; x < tile_width; x++) {
                    int px = tile_px_x + x;
                    int py = tile_px_y + y;
                    int off = py * stride + px;
                    int sx = !flip ? x : tile_width - x - 1;
                    int soff = y * tile_width + sx;

                    dst[off] = src[soff];
                }
            }
        }
    }

    ds_trampoline_end();
}

void far pascal render_all_background_layers() {

    render_background_layer_mine(0);
    render_background_layer_mine(1);
    render_background_layer_mine(2);
    render_background_layer_mine(9);

    ds_trampoline_start();
    {
    char *xx = "room xx";
    format_byte(xx + 5, *current_room_number);    
    bg_dump(0, 190, xx);
    }
    ds_trampoline_end();
}

void far pascal animate_and_render_background() {
    int i;
    ds_trampoline_start();

    for (i = 0; i < 0x14 * 0x14; i++) {
        unsigned int tile_o = from_big_endian(tiles_ids[i]);
        unsigned int tile_n = tile_o;
        unsigned int frame  = *background_ani_frame;

        if ((tile_n & 0xf000) == 0x9000) {
            if (tile_n & 0x0800) {
                frame = 3;
            }
            else {
                if ((tile_n & 0x0400) && (frame == 3)) {
                    tile_n = tile_n | 0x0800;
                }
            }
        
            tile_n = (tile_n & 0xfffc) | (frame & 0x0003);
            tiles_ids[i] = from_big_endian(tile_n);
        } 
    }

    *background_ani_countdown = *background_ani_countdown - 1;

    if (*background_ani_countdown == 0) {
        *background_ani_frame = *background_ani_frame + 1;
        *background_ani_countdown = 4;
    }

    if (*background_ani_frame == 4) {
        *background_ani_frame = 0;
    }

    ds_trampoline_end();
    render_all_background_layers();
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

void far pascal render_pause_box(char far* string, int color, int boh) {
    get_text_width_t gtw;
    render_string_t rs;
    gfx_1_t g1;
    gfx_2_t g2;
    gfx_3_t g3;

    unsigned int old_buffer;
    int text_width;
    int left;
    int right;
    int y;

    ds_trampoline_start();

    gtw = get_text_width;
    rs = render_string;
    g1 = gfx_1;
    g2 = gfx_2;
    g3 = gfx_3;

    old_buffer = *background_buffer;
    *background_buffer = 0xa000;

    ds_trampoline_end();
    g1(0, 0, 0x13f, 0xc7);
    ds_trampoline_start();

    ds_trampoline_end();
    text_width = gtw(string);
    ds_trampoline_start();

    left = (0x140 - text_width) / 2;
    right = left + text_width;

    y = 0x5c;

    if (left <     4) left =     4;
    if (left > 0x13b) left = 0x13b;

    ds_trampoline_end();
    g2(left - 4, y, right + 3, y + 0x10, boh);
    ds_trampoline_start();

    ds_trampoline_end();
    g3(left - 3, y + 1, right + 2, y + 0xf, color);
    ds_trampoline_start();

    ds_trampoline_end();
    rs(left, y + 5, string, color);
    ds_trampoline_start();

    *background_buffer = old_buffer;
    ds_trampoline_end();
}

void far pascal mouse_pointer_init() {
    ds_trampoline_start();
    *mouse_pointer_x         = 0xa8;
    *mouse_pointer_y         = 0x64;
    *mouse_pointer_file_size = 0x00;
    ds_trampoline_end();
}

void far pascal mouse_check_buttons() {
    int button_id;
    int button;
    int x;
    int y;
    int count;

    ds_trampoline_start();

    if (*mouse_inited) {
        mouse_button_status_t ms = mouse_button_status;
        mouse_click_event_t evt = mouse_click_event;
        cammina_per_click_t camm = cammina_per_click;

        void far* mousefunc2 = *mouse_funcptr2;
        ds_trampoline_end();

        button_id = 0;
        button = 1;
        ms(&button, &x, &y, &count);

        ds_trampoline_start();

        if (count > 0) {
            button_id = 2;
        }
        else {
            ds_trampoline_end();
            button = 0;
            ms(&button, &x, &y, &count);
            ds_trampoline_start();

            if (count > 0) {
                button_id = 1;
            }
        }

        if (button_id) {
            int mp_x = *mouse_pointer_x;
            int mp_y = *mouse_pointer_y;

            ds_trampoline_end();
            evt(mp_x, mp_y, button_id);
            ds_trampoline_start();
        }

        if (*mouse_do_default_action) {
            int ani = *pupo_current_ani;
            int (far pascal *funcptr2)(int, int) = mousefunc2;

            ds_trampoline_end();

            if (funcptr2(ani, 0)) {
                ds_trampoline_start();
                ds_trampoline_end();
                camm();
            }

            ds_trampoline_start();
        }
    }

    ds_trampoline_end();
}

void far pascal mouse_set_position(int col, int row) {
    asm mov ax, 4;
    asm mov cx, col;
    asm mov dx, row;
    asm int 33h;
}

void far pascal mouse_pointer_draw() {
    ds_trampoline_start();

    if ((*mouse_pointer_file_size) &&
        (*mouse_inited))
    {
        mouse_get_status_t gs = mouse_get_status;

        int dx;
        int dy;

        int status;

        ds_trampoline_end();

        gs(&dx, &dy, &status);
        ds_trampoline_start();

        mouse_set_position(0xa0, 0x64);

        *mouse_pointer_x = *mouse_pointer_x + dx - 0xa0; 
        *mouse_pointer_y = *mouse_pointer_y + dy - 0x64;

        if (*mouse_pointer_x > 0x13d) *mouse_pointer_x = 0x13d;
        if (*mouse_pointer_x < 0    ) *mouse_pointer_x = 0x0;

        if (*mouse_pointer_y >  0xc5) *mouse_pointer_y = 0xc5;
        if (*mouse_pointer_y <  0   ) *mouse_pointer_y = 0x0;

        {
            mouse_pointer_for_point_t mpt = mouse_pointer_for_point;

            int x;
            int y;
            void far* ele;
            int cursor;

            x = *mouse_pointer_x;
            y = *mouse_pointer_y;

            ds_trampoline_end();
            cursor = mpt(x, y);
            ds_trampoline_start();

            /* todo: override cursor based on set stru_13452 at seg003:0202 */

            x = *mouse_pointer_x;
            y = *mouse_pointer_y;
            ele = mouse_pointer_ele_1[cursor - 1];

            ds_trampoline_end();
            add_bob_per_background(x + 8, y + 10, ele, 0xffe1, 0, 0xc7);
            ds_trampoline_start();
        }
    }

    ds_trampoline_end();
}

void far pascal do_tiletype_actions_inner(unsigned int boh) {
    int idx;
    int top;
    int i;
    void far* ptr1;
    void far* ptr2;
    int type;

    ds_trampoline_start();

    idx = boh == 0xff ? 0    : boh;
    top = boh == 0xff ? 0x3f : boh;

    for (i = idx; i <= top; i++) {

        if ((*swivar_block_2)[i] == 0)
            continue;

        ptr1 = swi_file_content[i];
        type = *((int  far*)ptr1);
        ptr2 = (((char far*)ptr1) + 4);
        type = from_big_endian(type); 

        if (type != 0) {
            tiletype_action_t ac;

            *background_ani_frame = 0;

            ac = ((tiletype_action_t far*)tiletype_actions)[type - 1];

            if (0)
            {
                char *dc_i   = "i    xx";
                char *dc_swi = "swi  xxxx:xxxx";
                char *dc_toc = "ac   xxxx:xxxx";
                char *dc_p1  = "p1   xxxx:xxxx";
                char *dc_p2  = "p2   xxxx:xxxx";
                char *diocan = "type xxxx";
                char Y = 0;

                format_byte(dc_i + 5, i);
                vga_dump(0, Y, dc_i);
                Y += 8;

                format_ptr(dc_swi + 5, swi_file_elements);
                vga_dump(0, Y, dc_swi);
                Y += 8;

                format_ptr(dc_p1 + 5, ptr1);
                vga_dump(0, Y, dc_p1);
                Y += 8;

                format_ptr(dc_p2 + 5, ptr2);
                vga_dump(0, Y, dc_p2);
                Y += 8;

                format_word(diocan + 5, type);
                vga_dump(0, Y, diocan);
                Y += 8;

                format_ptr(dc_toc + 5, ac);
                vga_dump(0, Y, dc_toc);
                Y += 8;

                wait_vsync();
                wait_vsync();
                wait_vsync();

                while(1);
            }

            ds_trampoline_end();
            ac(ptr2, &i, &top);
            ds_trampoline_start();
        }
    }

    ds_trampoline_end();
}

void far pascal do_tiletype_actions(char boh) {

    if (boh > 0x3f)
        return;

    ds_trampoline_start();

    if ((*swivar_block_2)[boh] == 0) {
        do_tiletype_actions_inner_t in = do_tiletype_actions_inner_th;

        (*swivar_block_2)[boh] = 1;

        ds_trampoline_end();
        if (1) {
            in(boh);
        } else {
            do_tiletype_actions_inner(boh);
        }
        ds_trampoline_start();
    }

    ds_trampoline_end();
}

void far pascal draw_faccia() {
    int y;
    int boh;
    void far *ele1;
    void far *ele0;

    ds_trampoline_start();

    if (*faccia_countdown == 0) {
        ds_trampoline_end();
        return;
    }

    *faccia_countdown = *faccia_countdown - 1;

    y = *pupo_y < 0x64 ? 0xc3 : 0x34;
    boh = y - 7 - ((0x800 - *vita) / 0x31);

    ele1 = (*status_ele_block)[1];
    ele0 = (*status_ele_block)[0];

    {
        char *suca;

        gfx_2_t g2 = gfx_2;

        ds_trampoline_end();
        g2(0, 98, 70, 2 * 8 + 100 + 2, 50);

        ds_trampoline_start();
        suca  = "vita xxxx";
        format_word(suca + 5, *vita);
        bg_dump(20, 100, suca);

        suca  = "boh  xxxx";
        format_word(suca + 5, boh);
        bg_dump(20, 108, suca);
    }

    { 
        ds_trampoline_end();
        add_bob_per_background(0x123, y, ele1, 0xfff0, 0, 0);
        add_bob_per_background(0x123, y + 50, ele0, 0xfff0, 0, boh);
        ds_trampoline_start();
    }

    ds_trampoline_end();
}

void far pascal calls_funcptr_1() {
    void (far pascal *impl)() = MK_FP(seg004, 0x0cd4);

    ds_trampoline_end();
    impl();
    ds_trampoline_start();
}

char far pascal capisci_dove_muovere_il_pupo_key() {
    /*
        +------  down
        |+-----  up
        ||+----  left
        |||+---  right
        ||||
        ----
        0000 -> 0
        0001 -> 3
        0010 -> 7
        0011 ->        [0]
        0100 -> 1
        0101 -> 2
        0110 -> 8
        0111 ->        [1]
        1000 -> 5
        1001 -> 4
        1010 -> 6
        1011 ->        [5]
        1100 ->        [0]
        1101 ->        [3]
        1110 ->        [4]
        1111 ->        [0]
    */

    static char dpad_to_direction[0x10] = {
        0, 3, 7, 0, 1, 2, 8, 1, 5, 4, 6, 5, 0, 3, 4, 0
    };

    char dpad;
    char direction;
    char shift;

    ds_trampoline_start();

    dpad =
        (!!*down_pressed  << 3) |
        (!!*up_pressed    << 2) |
        (!!*left_pressed  << 1) |
        (!!*right_pressed << 0);

    shift = (!!*l_shift_pressed) || (!!*r_shift_pressed);

    direction = dpad_to_direction[dpad] + (shift ? 9 : 0);

    if (direction) {
        reset_clicked_button_t rese = reset_clicked_button;

        ds_trampoline_end();
        rese();
        ds_trampoline_start();
    }
    else {
        char (far pascal *muove_pupo_per_mouse)() = MK_FP(seg004, 0x018d);

        ds_trampoline_end();
        direction = muove_pupo_per_mouse();
        ds_trampoline_start();
    }
    
    ds_trampoline_end();

    return direction;
}

void far pascal copy_bg_to_vga() {
    void (far pascal *impl)() = MK_FP(seg015, 0x1435);

    ds_trampoline_end();
    impl();
    ds_trampoline_start();
}

void far pascal controlla_sotto_piedi(
    char far* ani,
    int top,
    int bottom,
    int x,
    int y,
    int new_x,
    int new_y
) {
    char direction = *pupo_current_ani >= 0x35;
    char var2 = 0;

    goto original;
    return;

    if (*byte_1f4e6 == 0 && top == 0xfc) {
        /* loc_11e91 */
        goto original;
    }

    if (logi_tab_contains(bottom, 0x25)) {
        /* loc_11ea8 */
        goto original;
    }

    if (*byte_1f4e6 < 0) {
        /* loc_11ecf */
        goto original;
    }

    if (top == 9) {
        /* fai cade */
        goto original;
    }

    if (top == 0) {
        /* loc_11d10 */
        goto original;
    }

    if (*counter_caduta > 0) {
        /* loc_11f3c */
        goto original;
    }

    #if 0
    if (*byte_1f4e8 < 0) {
        *byte_1f4e8 = *byte_1f4e8 - 1;
        if (set_is_member(*ani, MK_FP(seg002, 0x0E79))) {
            /* set ani 38_3 */
            var2 = 1;
        }
    }

    if (!var2) {
        if (*byte_1f4e9 > 0) {
            *byte_1f4e9 = *byte_1f4e9 - 1;
        }
        if (set_is_member(*ani, MK_FP(seg002, 0x0e99))) {
            /* set ani 38_3 */
            var2 = 1;
        }
    }

    if (var2) {
        void (far pascal *cambia_il_salto)(char far* ani, char tile_top)
            = MK_FP(seg002, 0x022a);

        ds_trampoline_end();
        cambia_il_salto(ani, top);
        ds_trampoline_start();
    }
    #endif 
    if ((*ani == 0x12) || (*ani == 0x13)) {
        /* sub_11a14 */
    }

    if ((*ani == 0x47) || (*ani == 0x48)) {
        /* sub_11a3d */
    }

    if (set_is_member(*ani, MK_FP(seg002, 0x0e79))) {
        /* loc_11ff6 */
        goto original;
    }

    if (set_is_member(*ani, MK_FP(seg002, 0x0e99))) {
        /* loc_12011 */
        goto original;
    }

    if (*ani == 0x14) {
        /* loc_12027 */
        goto original;
    }

    if (*ani == 0x49) {
        /* loc_12036 */
        goto original;
    }

    if (*ani == 0x8) {
        /* loc_12045 */
        goto original;
    }

    if (*ani == 0x9) {
        /* loc_12054 */
        goto original;
    }

    if (*ani == 0x3d) {
        /* loc_12063 */
        goto original;
    }

    if (*ani == 0x3e) {
        /* loc_12072 */
        goto original;
    }

    if (*ani == 0x10) {
        /* loc_1207d */
        goto original;
    }

    if (*ani == 0x45) {
        /* loc_12088 */
        goto original;
    }

    if ((*ani == 0x1f) ||
        (*ani == 0x20) ||
        (*ani == 0x54))
    {
        /* loc_12094 */
        goto original;
    }

    if ((*ani == 0x21) ||
        (*ani == 0x55))
    {
        /* loc_120a3 */
        goto original;
    }

    return;

original:

    {
        void (far pascal *controlla_sotto_piedi)(void far* ani, int top, int bottom, int x, int y, int new_x, int new_y)
            = MK_FP(seg002, 0x0eb9);

        ds_trampoline_end();
        controlla_sotto_piedi(ani, top, bottom, x, y, new_x, new_y);
        ds_trampoline_start();

        return;
    }
}

void far pascal cambia_il_salto(unsigned char far* ani, int top) {
    ds_trampoline_start();

    {
        char far* sostani = *sostani_file;
        int offset = *(int far*)(sostani);
        char far* sostani_item = sostani + from_big_endian(offset);

        unsigned char oldani;

        while (1) {
            oldani = *(sostani_item);

            if (oldani == 0xff) {
                break;
            }

            if (oldani == *ani)
            {
                char logitab_index;
                unsigned char new_ani;

                logitab_index = *(sostani_item + 1);
                new_ani = *(sostani_item + 2);

                if (logi_tab_contains(top, logitab_index))
                    *ani = new_ani;
            }

            sostani_item += 3;
        }
    }
    ds_trampoline_end();
}

void far pascal cosa_ho_in_fronte(char far* ani, int x, int y, int nx, int ny) {
    char far* sostani = *sostani_file;
    int offset = *(int far*)(sostani + 2);
    char far* sostani_item = sostani + from_big_endian(offset);

    char new_ani = *ani;
    char changed = 0;

    char ani_from_file;
    char logitab_idx;
    char x_offset;

    char not_found = 0;

    int xoff, i;

    int incr;
    unsigned char oldani;

    do {
        changed = 0;
        not_found = 0;

        while (1) {
            oldani = *(sostani_item);

            if (oldani == 0xff) {
                not_found = 1;
                break;
            }

            if (oldani == new_ani)
                break;

            sostani_item += 4;
        }

        if (not_found)
            continue;

        ani_from_file = *(sostani_item + 1);
        logitab_idx = *(sostani_item + 2);
        x_offset = *(sostani_item + 3);

        xoff = nx - x + x_offset;
        xoff = xoff >  0x40 ? xoff + 0x10 : xoff;
        xoff = xoff < -0x40 ? xoff - 0x10 : xoff;

        incr = xoff < 0 ? 0x10 : -0x10;

        while (xoff != 0) {
            char tile_type = get_tile_type(x + xoff, y);

            if (logi_tab_contains(tile_type, logitab_idx)) {
                new_ani = ani_from_file;
                changed = 1;
                break;
            }

            xoff += incr;
        }
    } while (changed);

    *ani = new_ani;
}

void far pascal offset_from_ani(int ani, int x, int y, int far* nx, int far* ny) {
    char far* animofs = *animofs_tab_file;

    int array0 = from_big_endian(*(int far*)(animofs + 0));
    int array1 = from_big_endian(*(int far*)(animofs + 2));
    int array2 = from_big_endian(*(int far*)(animofs + 4));
    int array3 = from_big_endian(*(int far*)(animofs + 6));

    *nx = x + *(animofs + array2 + ani) + *(animofs + array0 + ani);
    *ny = y + *(animofs + array3 + ani) + *(animofs + array1 + ani);
}

void far update_pupo_1() {
    ds_trampoline_start();

    *pupo_tile_top    = get_tile_type(*pupo_x, *pupo_y     );
    *pupo_tile_bottom = get_tile_type(*pupo_x, *pupo_y + 10);

    controlla_sotto_piedi(
        pupo_current_ani,
        *pupo_tile_top,
        *pupo_tile_bottom,
        *pupo_x,
        *pupo_y,
        *pupo_new_x,
        *pupo_new_y
    );

    if (*gun_bool == 0) {
        int a = *pupo_current_ani;

        *pupo_current_ani =
            (a == 0x47) ? 0x48:
            (a == 0x12) ? 0x13:
            a;
    }

    {
        int a = *pupo_current_ani;

        do {
            offset_from_ani(*pupo_current_ani, *pupo_x, *pupo_y, pupo_new_x, pupo_new_y);

            {
                void (far pascal *cosa_ho_di_fronte)(char far* ani, int x, int y, int nx, int ny)
                    = MK_FP(seg002, 0x114c);

                char __ani = *pupo_current_ani;
                int NX = *pupo_new_x;
                int NY = *pupo_new_y;
                int X = *pupo_x;
                int Y = *pupo_y;

/*
                ds_trampoline_end();
                cosa_ho_di_fronte(&__ani, X, Y, NX, NY);
                ds_trampoline_start();
*/
                cosa_ho_in_fronte(&__ani, X, Y, NX, NY);

                *pupo_current_ani = __ani;
                a = __ani;
            }
        } while (*pupo_current_ani != a);
    }

    ds_trampoline_end();
}

void far pascal update_pupo_2() {
    char far* frames_tab_file;

    char far* ani;
    int a;
    int d;
    int o;

    ds_trampoline_start();

    frames_tab_file = MK_FP(*frames_tab_file_seg, *frames_tab_file_off);
 
    *byte_1f4dc = 0;
    *pupo_offset = from_big_endian(*(int far *)(frames_tab_file + (*pupo_current_ani * 2)));

    {
        char far* animofs = *animofs_tab_file;

        int  far* offs = (int far* )animofs;
        char      ani  = *pupo_current_ani;
        int       o1   = from_big_endian(offs[0]) + ani;
        int       o2   = from_big_endian(offs[1]) + ani;

        *pupo_x = *pupo_x + *(animofs + o1);
        *pupo_y = *pupo_y + *(animofs + o2);
    }

    ds_trampoline_end();
}

void far pascal update_pupo_3() {
    struct frame_info_t {
        char frame;
        char countdown;
        char x_delta;
        char y_delta;
        char flip;
    };

    char far* frames_tab_file;
    struct frame_info_t far* frame_info;

    ds_trampoline_start();

    frames_tab_file = MK_FP(*frames_tab_file_seg, *frames_tab_file_off);
    frame_info = (struct frame_info_t far *)(frames_tab_file + *pupo_offset);

    if ((frame_info->frame | frame_info->countdown)) {
        *pupo_offset = *pupo_offset + 5; /* sizeof(frame_info_t) */
        *pupo_current_frame = frame_info->frame;
        *pupo_anim_countdown = (frame_info->countdown + 3) / 4;
        *pupo_x_delta = frame_info->x_delta;
        *pupo_y_delta = frame_info->y_delta;
        *pupo_flip = frame_info->flip;
    }
    else {
        *get_new_ani = 1;
    }

    if (*get_new_ani) {
        char far* animofs = *animofs_tab_file;

        int  far* offs = (int far* )animofs;
        char      ani  = *pupo_current_ani;
        int       o1   = from_big_endian(offs[2]) + ani;
        int       o2   = from_big_endian(offs[3]) + ani;

        *pupo_x = *pupo_x + *(animofs + o1);
        *pupo_y = *pupo_y + *(animofs + o2);

        if ((*to_set_pupo_x < 0) ||
            (*to_set_pupo_x > 319))
        {
            *to_set_pupo_x = *pupo_x;
            *to_set_pupo_y = *pupo_y;
        }
    }

    ds_trampoline_end();
}

void far pascal update_pupo_4() {

    ds_trampoline_start();
    *byte_1f4dc = *byte_1f4dc + 1;

    if ((*pupo_tile_top & 0xf0) == 0xd0)
    {
        reset_clicked_button_t rese = reset_clicked_button;

        char type = (*pupo_tile_top & 0x0f) + 0xc;

        ds_trampoline_end();
        do_tiletype_actions(type);
        rese();
        ds_trampoline_start();
        calls_funcptr_1();

        {
            void (far pascal *reset_array_bobs)() = MK_FP(seg013, 0x0685);
            void (far pascal *reset_background_ani)() = MK_FP(seg013, 0x4cf);

            *pupo_new_x = *pupo_x;
            *pupo_new_y = *pupo_y;

            *pupo_tile_top    = get_tile_type(*pupo_x, *pupo_y     );
            *pupo_tile_bottom = get_tile_type(*pupo_x, *pupo_y + 10);

            ds_trampoline_end();
            reset_array_bobs();
            reset_background_ani();
            ds_trampoline_start();

            if (*vita > 0x800) {
                void (far pascal *draw_punti_faccia)() = MK_FP(seg012, 0x05e6);
                void (far pascal *draw_stars)() = MK_FP(seg005, 0x0000);

                ds_trampoline_end();
                animate_and_render_background();
                draw_stars();
                draw_punti_faccia();
                ds_trampoline_start();
            }

            {
                void (far pascal *select_pal)(char a, char b) = MK_FP(seg017, 0x0916);
                void (far pascal *install_ucci)() = MK_FP(seg011, 0x0486);

                copy_bg_to_vga();

                ds_trampoline_end();
                select_pal(1, 0);
                install_ucci();
                ds_trampoline_start();

                *palette_mangling_counter = 2;
            }
        }
    }

    ds_trampoline_end();
}

void far pascal get_room_from_files(int room) {
    void (far pascal *impl)(int) = MK_FP(seg013, 0x03e6);

    ds_trampoline_end();
    impl(room);
    ds_trampoline_start();
}

void far pascal check_and_load_ucci() {
    void (far pascal *impl)() = MK_FP(seg011, 0x311);

    ds_trampoline_end();
    impl();
    ds_trampoline_start();
}


int YYY = 0;


void far pascal change_room(int room_to_change) {
    char previous_room;

    ds_trampoline_start();

    {
        reset_clicked_button_t rese = reset_clicked_button;

        ds_trampoline_end();
        rese();
        ds_trampoline_start();
    }

    *pupo_new_x = *pupo_x;
    *pupo_new_y = *pupo_y;

    previous_room = *no_previous_room ? 0 : *current_room_number;
    *wanted_room = room_to_change;

    if (!*need_to_save_arc_lzr && !*maybe_exit_related) {
        void (far pascal *gsa_and_exit_room)(int)
            = MK_FP(seg006, 0x0b4a);

        int room = *wanted_room;

        ds_trampoline_end();
        gsa_and_exit_room(room);
        ds_trampoline_start();
    }
   
    *pupo_new_x = *pupo_x;
    *pupo_new_y = *pupo_y;

    if (*vita >= 0x800) {
        if (!*no_previous_room) {
            room_to_change = *wanted_room;
            get_room_from_files(*wanted_room);
            calls_funcptr_1();
        }
    }
    else {
        room_to_change = *wanted_room;
        get_room_from_files(*wanted_room);

        if (*read_from_usc) {
            char far* current_usc = *usc_file;

            while (1) {
                unsigned char room_from = *(current_usc + 0);
                unsigned char room_to   = *(current_usc + 1);
                
                if (room_from == 0xff && room_to == 0xff) {
                    break;
                }

                if ((room_from == previous_room) &&
                    (room_to == room_to_change))
                {
                    int y_from = from_big_endian(*(int far*)(current_usc + 4));
                    int y_to   = from_big_endian(*(int far*)(current_usc + 2));

                    vga_dump_word(&YYY, "from", previous_room);
                    vga_dump_word(&YYY, "to", room_to_change);


                    if (y_from == *pupo_new_y) {
                        *pupo_new_y = y_to;
                        *pupo_y = y_to;

                        vga_dump_word(&YYY, "oldY", y_from);
                        vga_dump_word(&YYY, "newY", y_to);

                        wait_vsync();
                        wait_vsync();
                        wait_vsync();
                        wait_vsync();

                        break;
                    }
                }

                current_usc += 6;
            }
        }
        else {
            /* read from prt */
        }

        *to_set_pupo_x = *pupo_new_x;
        *to_set_pupo_y = *pupo_new_y;

        {
            do_tiletype_actions_inner_t in = do_tiletype_actions_inner_th;

            ds_trampoline_end();
            in(0xffff);
            ds_trampoline_start();
        }
    }

    calls_funcptr_1();
    
    *pupo_tile_top    = get_tile_type(*pupo_new_x, *pupo_new_y     );
    *pupo_tile_bottom = get_tile_type(*pupo_new_x, *pupo_new_y + 10);

    if (!*read_from_usc) {
        /* read from dsp etc */
    }

    *read_from_usc = 0;
 
    {
        void (far pascal *reset_array_bobs)() = MK_FP(seg013, 0x0685);
        void (far pascal *reset_background_ani)() = MK_FP(seg013, 0x4cf);
        void (far pascal *draw_punti_faccia)() = MK_FP(seg012, 0x05e6);
        void (far pascal *draw_stars)() = MK_FP(seg005, 0x0000);

        *no_previous_room = 0;

        ds_trampoline_end();
        reset_array_bobs();
        ds_trampoline_start();

        check_and_load_ucci();

        ds_trampoline_end();
        reset_background_ani();
        ds_trampoline_start();

        if (*vita <= 0x800) {
            ds_trampoline_end();
            animate_and_render_background();
            draw_punti_faccia();
            draw_stars();
            ds_trampoline_start();
        }

        copy_bg_to_vga();

        (*swivar_block_1)[*current_room_number + 0x14] = 1;

        if (!*should_stop_gameloop) {
            if (!*disable_pupo_anim) {
                void (far pascal *maybe_fade)(int) = MK_FP(seg017, 0x0026);

                ds_trampoline_end();
                maybe_fade(1);
                ds_trampoline_start();
            }
        }
    }


    ds_trampoline_end();
}

void far pascal change_at_screen() {
    char direction = *pupo_current_ani >= 0x35;

    int pupo_real_x = *pupo_x + *pupo_x_delta;

    if ((pupo_real_x > 319 && direction) ||
        (pupo_real_x < 0 && !direction))
    {
        void far* stru_121f0 = MK_FP(seg002, 0x1290);

        if (!set_is_member(*pupo_current_ani, stru_121f0)) {
            *read_from_usc = 1;

            if (*pupo_x > 0)
                *pupo_new_x = *pupo_x - 320;
            else
                *pupo_new_x = *pupo_x + 320;

            if (pupo_real_x > 0)
                *pupo_x = 312;
            else
                *pupo_x = 8;

            {
                int y = *pupo_y - 0xa;
                char new_room;

                do {
                    char tile_type;

                    new_room = get_tile_type(*pupo_x, y);
                    y += 0xa;

                    tile_type = get_tile_type(*pupo_x, y);
                    y += 0xa;

                    if (logi_tab_contains(tile_type, 0x25))
                        break;
                } while (y <= 0xc7);

                *pupo_x = *pupo_new_x;

                if (0)
                {
                    void (far pascal *real_change_room)(int room)
                         = MK_FP(seg002, 0x02cb);

                    ds_trampoline_end();
                    real_change_room(new_room);
                    ds_trampoline_start();
                }
                else {
                    ds_trampoline_end();
                    change_room(new_room);
                    ds_trampoline_start();
                }
            }
        }
    }
}

void far pascal update_pupo() {
    char enemy_hit = 0;
    char get_new_frame;
    char far* animjoy_tab_file;

    ds_trampoline_start();

    animjoy_tab_file = MK_FP(*animjoy_tab_file_seg, *animjoy_tab_file_off);
    do {

        if (!enemy_hit)
            get_new_frame = 0;

        if (*get_new_ani && !enemy_hit)
        {
            char far* new_ani;
            char dove;

            if (*esc_pressed_flag) {
                dove = 0;
            }
            else {
                ds_trampoline_end();
                dove = capisci_dove_muovere_il_pupo_key();
                ds_trampoline_start();
            }

            new_ani = animjoy_tab_file + (*pupo_current_ani * 18) + dove;
            *pupo_current_ani = *new_ani;

            *get_new_ani = 0;
            get_new_frame = 1;

            ds_trampoline_end();
            update_pupo_1();
            ds_trampoline_start();
        }

        if (*disable_pupo_anim) {
            get_new_frame = 1;
            *disable_pupo_anim = 0;
        }

        {
            void (far pascal *gsa_and_exit)() = MK_FP(seg006, 0x0ce5);

            ds_trampoline_end();
            gsa_and_exit();
            ds_trampoline_start();
        }

        if (*pupo_anim_countdown == 0) {
            if (get_new_frame) {
                ds_trampoline_end();
                update_pupo_2();
                ds_trampoline_start();
            }

            ds_trampoline_end();
            update_pupo_3();
            update_pupo_4();
            ds_trampoline_start();

            {
                void (far pascal *sub_1243d)() = MK_FP(seg002, 0x14dd);

                ds_trampoline_end();
                sub_1243d();
                ds_trampoline_start();
            }

            {
                char (far pascal *check_pu_for_vita)(int x) = MK_FP(seg011, 0x0557);
                enemy_hit = 1;

                ds_trampoline_end();
                if (!check_pu_for_vita(0) && !check_pu_for_vita(1)) {
                    enemy_hit = 0;
                }
                ds_trampoline_start();


                if (enemy_hit) {
                    *get_new_ani = 1;
                    get_new_frame = 1;
                    *pupo_anim_countdown = 0;
                    *pupo_current_ani = *pupo_current_ani > 0x35 ? 0x51: 0x1c;
                }
                else {
                    if (*byte_1f4dc == 4) {
                        if (set_is_member(*pupo_current_ani, MK_FP(seg002, 0x176d))) {
                            reset_clicked_button_t rese = reset_clicked_button;
                            void (far pascal *mangle_pu_gun)() = MK_FP(seg011, 0x0619);

                            ds_trampoline_end();
                            mangle_pu_gun();
                            rese();
                            ds_trampoline_start();
                        }
                    }
                }
            }
        }
    } while (*get_new_ani);


    {
        void (far pascal *sub_122f1)() = MK_FP(seg002, 0x1391);

        change_at_screen();

        ds_trampoline_end();
        sub_122f1();
        ds_trampoline_start();

        if (!*disable_pupo_anim) {
            *pupo_anim_countdown = *pupo_anim_countdown - 1;
        }
    }

    {
        char *suca = "xxxx";
        gfx_2_t g2 = gfx_2;

        ds_trampoline_end();
        g2(150, 190, 319, 199, 50);
        ds_trampoline_start();
        format_word(suca, *pupo_x);
        bg_dump(200, 190, suca);
        format_word(suca, *pupo_y - 10);
        bg_dump(250, 190, suca);
        format_word(suca, *pupo_current_ani);
        bg_dump(150, 190, suca);
    }

    ds_trampoline_end();
}

void far pascal add_enemy_to_bobs(int enemy_id) {
    struct pu_item_t {
        int  x1;
        int  y1;
        int  x2;
        int  y2;
        char ignored1;
        char ignored2;
        int  boh1;
        char bool1;
        char maybe_item_type;
        int  copy_x1;
        int  copy_y1;
        char sprite_nr;
        char ignored3;
        char ignored4;
        char ignored5;
        int  wanted_ucci_nr;
        int  unk27;
    };

    ds_trampoline_start();

    {
        int offset = (*current_room_number * 2 + enemy_id) * 0x1c;
        unsigned int ptr_off = *pu_file_buffer_off + offset;
        char far* pu = (char far*)MK_FP(*pu_file_buffer_seg, ptr_off);
        struct pu_item_t far* pu_item = (struct pu_item_t far*)pu;

        char sprite_nr = *(pu + 20);
        char far* ele = (*ucci_image_content)[sprite_nr];

        int  x = pu_item->x1 + pu_item->x2;
        int  y = pu_item->y1 + pu_item->y2;
        char flip = enemy_flip[enemy_id];

        /* check on ucci_image_content should not be here, but for now
        it seems i need it */
        if (*ucci_image_content && (!pu_item->bool1 || x < 320)) {
            ds_trampoline_end();
            add_bob_per_background(x, y, ele, 0xffd1, flip, 0);
            ds_trampoline_start();
        }
    }

    ds_trampoline_end();
}

void far pascal add_pupo_to_bobs() {
    int x;
    int y;
    void far* ele;
    int flip;
    int color;

    ds_trampoline_start();

    x = *pupo_x + *pupo_x_delta;
    y = *pupo_y + *pupo_y_delta;
    ele = (*tr_ele_file)[*pupo_current_frame];
    flip = *pupo_flip;
    color = *pupo_palette_override;

    ds_trampoline_end();
    add_bob_per_background(x, y, ele, 0xffc1, flip, color);
    ds_trampoline_start();

    *bob_to_hit_mouse_3 = *bobs_count;
    
    ds_trampoline_end();
}

void far pascal draw_pupi() {
    /* sort not implemented */

    add_enemy_to_bobs(0);
    add_enemy_to_bobs(1);
    add_pupo_to_bobs();
}

void init_pointers() {
    highlight_frame_nr       = MK_FP(dseg, 0x0100);
    to_set_pupo_x            = MK_FP(dseg, 0x08fc);
    to_set_pupo_y            = MK_FP(dseg, 0x08fe);
    animofs_tab_file         = MK_FP(dseg, 0x0930);
    animjoy_tab_file_seg     = MK_FP(dseg, 0x0934);
    animjoy_tab_file_off     = MK_FP(dseg, 0x0936);
    frames_tab_file_seg      = MK_FP(dseg, 0x0938);
    frames_tab_file_off      = MK_FP(dseg, 0x093a);
    byte_1f4dc               = MK_FP(dseg, 0x094c);
    pupo_current_frame       = MK_FP(dseg, 0x094d);
    pupo_flip                = MK_FP(dseg, 0x094e);
    pupo_x_delta             = MK_FP(dseg, 0x094f);
    pupo_y_delta             = MK_FP(dseg, 0x0950);
    pupo_offset              = MK_FP(dseg, 0x0952);
    byte_1f4e6               = MK_FP(dseg, 0x0956);
    pupo_palette_override    = MK_FP(dseg, 0x095a);
    tr_ele_file              = MK_FP(dseg, 0x095e);
    pupo_tile_top            = MK_FP(dseg, 0x0954);
    pupo_tile_bottom         = MK_FP(dseg, 0x0955);
    mouse_funcptr2           = MK_FP(dseg, 0x097c);
    mouse_pointer_file_size  = MK_FP(dseg, 0x0980);
    mouse_pointer_file_buf   = MK_FP(dseg, 0x0982);
    mouse_pointer_ele_1      = MK_FP(dseg, 0x0986);
    mouse_pointer_ele_2      = MK_FP(dseg, 0x098a);
    mouse_pointer_ele_3      = MK_FP(dseg, 0x098e);
    mouse_pointer_x          = MK_FP(dseg, 0x0992);
    mouse_pointer_y          = MK_FP(dseg, 0x0994);
    mouse_do_default_action  = MK_FP(dseg, 0x0998);
    tiletype_actions         = MK_FP(dseg, 0x2c9c);
    swi_file_content         = MK_FP(dseg, 0x2b98);
    swi_file_elements        = MK_FP(dseg, 0x2b9c);
    pti_file_content         = MK_FP(dseg, 0x2cbc);
    pupo_new_x               = MK_FP(dseg, 0x2cc4);
    pupo_new_y               = MK_FP(dseg, 0x2cc6);
    swivar_block_2           = MK_FP(dseg, 0x2eee);
    pupo_current_ani         = MK_FP(dseg, 0x2efb);
    get_new_ani              = MK_FP(dseg, 0x2efe);
    gun_bool                 = MK_FP(dseg, 0x2eff);
    pupo_x                   = MK_FP(dseg, 0x2ef6);
    pupo_y                   = MK_FP(dseg, 0x2ef8);
    pupo_anim_countdown      = MK_FP(dseg, 0x2efa);
    disable_pupo_anim        = MK_FP(dseg, 0x2efd);
    colpi                    = MK_FP(dseg, 0x2f00);
    vita                     = MK_FP(dseg, 0x2f04);
    punti                    = MK_FP(dseg, 0x2f06);
    status_ele_block         = MK_FP(dseg, 0x2f0c);
    palette_mangling_counter = MK_FP(dseg, 0x2f18);
    punti_countdown          = MK_FP(dseg, 0x2f29);
    faccia_countdown         = MK_FP(dseg, 0x2f2a);
    logi_tab_file            = MK_FP(dseg, 0x2f14);
    background_ani_frame     = MK_FP(dseg, 0x2f4f);
    background_ani_countdown = MK_FP(dseg, 0x2f4e);
    mat_filenames            = MK_FP(dseg, 0x2f58);
    tiles_ids                = MK_FP(dseg, 0x2f78);
    tiles_types              = MK_FP(dseg, 0x32b8);
    tiles_overrides          = MK_FP(dseg, 0x3448);
    bobs_ele_item            = MK_FP(dseg, 0x383c);
    bobs_sizeof              = MK_FP(dseg, 0x3904);
    bobs_palette_start       = MK_FP(dseg, 0x3968);
    bobs_flip                = MK_FP(dseg, 0x399a);
    bobs_color               = MK_FP(dseg, 0x39cc);
    bobs_x                   = MK_FP(dseg, 0x3af8);
    bobs_y                   = MK_FP(dseg, 0x3b5c);
    bobs_count               = MK_FP(dseg, 0x3bc0);
    background_buffer        = MK_FP(dseg, 0x3d20);
    bob_to_hit_mouse_3       = MK_FP(dseg, 0x376a);
    current_mat_loaded       = MK_FP(dseg, 0x3c14);
    mat_buffer               = MK_FP(dseg, 0x3770);
    mouse_inited             = MK_FP(dseg, 0x414b);
    esc_pressed_flag         = MK_FP(dseg, 0x414f);
    up_pressed               = MK_FP(dseg, 0x42a4);
    down_pressed             = MK_FP(dseg, 0x42a5);
    left_pressed             = MK_FP(dseg, 0x42a6);
    right_pressed            = MK_FP(dseg, 0x42a7);
    l_shift_pressed          = MK_FP(dseg, 0x42b1);
    r_shift_pressed          = MK_FP(dseg, 0x42b2);


    /* unsorted */
    current_room_number      = MK_FP(dseg, 0x2f50);
    pu_file_buffer_seg       = MK_FP(dseg, 0x2dc6);
    pu_file_buffer_off       = MK_FP(dseg, 0x2dc8);
    ucci_image_content       = MK_FP(dseg, 0x2dce);
    enemy_flip               = MK_FP(dseg, 0x2de8);
    bobs_to_hit_mouse        = MK_FP(dseg, 0x3768);
    read_from_usc            = MK_FP(dseg, 0x08fa);
    no_previous_room         = MK_FP(dseg, 0x08fb);
    wanted_room              = MK_FP(dseg, 0x2ef4);
    need_to_save_arc_lzr     = MK_FP(dseg, 0x0106); 
    maybe_exit_related       = MK_FP(dseg, 0x414d);
    swivar_block_1           = MK_FP(dseg, 0x2cc0);
    should_stop_gameloop     = MK_FP(dseg, 0x414c);
    dsp_file                 = MK_FP(dseg, 0x0920);
    usc_file                 = MK_FP(dseg, 0x0924);
    prt_file                 = MK_FP(dseg, 0x0928);
    sostani_file             = MK_FP(dseg, 0x092c);
    counter_caduta           = MK_FP(dseg, 0x0957);
    byte_1f4e8               = MK_FP(dseg, 0x0958);
    byte_1f4e9               = MK_FP(dseg, 0x0959);



    /* functions */

    mouse_pointer_for_point    = MK_FP(seg004, 0x078a);
    mouse_click_event          = MK_FP(seg004, 0x0851);
    cammina_per_click          = MK_FP(seg004, 0x08f0);
    do_tiletype_actions_inner_th
                               = MK_FP(seg006, 0x064b);
    get_line_from_pti_internal = MK_FP(seg008, 0x02c8);
    mouse_get_status           = MK_FP(seg010, 0x0022);
    mouse_button_status        = MK_FP(seg010, 0x0096);
    logi_tab_contains_theirs   = MK_FP(seg012, 0x0603);
    load_buffer_mat            = MK_FP(seg013, 0x0154);
    render_background_layer    = MK_FP(seg013, 0x0244);
    get_text_width             = MK_FP(seg015, 0x04fc);
    render_string              = MK_FP(seg015, 0x05f9);
    wait_vsync_theirs          = MK_FP(seg015, 0x1311);
    gfx_1                      = MK_FP(seg015, 0x136b);
    gfx_3                      = MK_FP(seg015, 0x1511);
    gfx_2                      = MK_FP(seg015, 0x1595);
    render_ele                 = MK_FP(seg015, 0x1b8e);
    render_ele_flipped         = MK_FP(seg015, 0x1bc3);
    reset_clicked_button       = MK_FP(seg004, 0x0000);

    patch_far_jmp(MK_FP(seg002, 0x0000), &capisci_dove_muovere_il_pupo_key); 
    patch_far_jmp(MK_FP(seg002, 0x178d), &update_pupo);
    patch_far_jmp(MK_FP(seg002, 0x1c2c), &draw_pupi);
    patch_far_jmp(MK_FP(seg003, 0x016e), &mouse_check_buttons);
    patch_far_jmp(MK_FP(seg003, 0x0222), &mouse_pointer_draw);
    patch_far_jmp(MK_FP(seg003, 0x0314), &mouse_pointer_init);
    patch_far_jmp(MK_FP(seg006, 0x070b), &do_tiletype_actions);
    patch_far_jmp(MK_FP(seg006, 0x0fcb), &render_context_explanation);
    patch_far_jmp(MK_FP(seg006, 0x101c), &render_key_help);
    patch_far_jmp(MK_FP(seg006, 0x10bb), &draw_highlight_under_cursor);
    patch_far_jmp(MK_FP(seg007, 0x001d), &get_line_from_pti);
    patch_far_jmp(MK_FP(seg012, 0x024a), &draw_faccia);
    patch_far_jmp(MK_FP(seg012, 0x0603), &logi_tab_contains_w);
    patch_far_jmp(MK_FP(seg013, 0x048d), &render_all_background_layers);
    patch_far_jmp(MK_FP(seg013, 0x04de), &animate_and_render_background);
    patch_far_jmp(MK_FP(seg013, 0x05e6), &screen_to_tile_x);
    patch_far_jmp(MK_FP(seg013, 0x0602), &screen_to_tile_y);
    patch_far_jmp(MK_FP(seg013, 0x061e), &get_tile_type_w);
    patch_far_jmp(MK_FP(seg013, 0x070a), &bobs_get_count);
    patch_far_jmp(MK_FP(seg013, 0x071d), &add_bob_per_background);
    patch_far_jmp(MK_FP(seg013, 0x0b64), &render_bobs_in_background);
    patch_far_jmp(MK_FP(seg015, 0x0c23), &render_pause_box);
    patch_far_jmp(MK_FP(seg015, 0x0eca), &render_help_string);
    patch_far_jmp(MK_FP(seg002, 0x022a), &cambia_il_salto);
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
    printf("dseg: %lx\n", dseg);
    printf("start...\n");
    getch();

    set_vga_mode();

    init_pointers();
    start_program();
}

