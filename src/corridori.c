#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#define PACKED __attribute__((packed))

const int GAME_SIZE_WIDTH  = 320;
const int GAME_SIZE_HEIGHT = 200;
const int GAME_SIZE_SCALE  =   3;

const int GAME_TILES_WIDTH  = 0x14;
const int GAME_TILES_HEIGHT = 0x14;

const int TILE_WIDTH  = 0x10;
const int TILE_HEIGHT = 0x0a;

typedef struct PACKED {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} tr_palette_color;

typedef struct PACKED {
    uint8_t start;
    uint8_t count;
    uint16_t padding;
    uint8_t boh;
    tr_palette_color palette[];
} tr_palette_file;

typedef struct {
    uint32_t color[0x100];
} tr_palette;

typedef struct {
    uint8_t *sets[0xc];
} tr_tilesets;

typedef struct {
    int width;
    int height;
    uint8_t *pixels;
    uint8_t *mask;
} tr_image_8bpp;

typedef struct {
    int count;
    tr_image_8bpp *items;
} tr_graphics;

typedef struct {
    tr_palette_file *arcade_pal;

    uint8_t *room_roe;
    uint8_t *buffer1;
    uint8_t *buffer2;
    uint8_t *buffer3;
    uint8_t *buffer4;
    uint8_t *buffer5;
    uint8_t *buffer6;
    uint8_t *bufferA;
    uint8_t *bufferB;

    uint8_t *status_ele;
    uint8_t *numeri_ele;
    uint8_t *k_ele;
    uint8_t *tr_ele;
    uint8_t *ucci0_ele;
    uint8_t *ucci1_ele;

    uint8_t *logi_tab;
    uint8_t *frames_tab;
    uint8_t *animjoy_tab;
    uint8_t *animofs_tab;

    uint8_t *dsp;
    uint8_t *usc;
    uint8_t *prt;
} tr_resources;

typedef struct {
    int16_t x;
    int16_t y;

    uint8_t  ani;
    uint8_t  ele_idx;

    bool     flip;
    int      countdown;

    // - //

    int8_t  x_delta;
    int8_t  y_delta;

    int16_t new_x;
    int16_t new_y;

    int16_t to_set_x;
    int16_t to_set_y;

    uint8_t  frame_nr;
    uint16_t pupo_offset;

    bool get_new_ani;
    bool disable_ani;

    // definitely not for her

    int  current_room;
    bool read_from_usc;
}  tr_pupo;

// - //

typedef struct {
    int count;
    Texture2D *textures;
    uint32_t  **data;
} ray_textures;

typedef struct {
    uint32_t data[GAME_SIZE_WIDTH * GAME_SIZE_HEIGHT];
    uint8_t background[GAME_SIZE_WIDTH * GAME_SIZE_HEIGHT];

    tr_resources *resources;
    tr_palette   *palette;
    tr_tilesets  *tilesets;
    Texture2D     texture;
} ray_bg_renderer;

// - //

uint16_t from_big_endian(uint16_t x) {
    return ((x & 0x00ff) << 8) | (x >> 8);
}

void *load_file(char *path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);

    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t *buf = malloc(size);
    fread(buf, size, 1, file);
    fclose(file);

    return buf;
}

void resources_load(tr_resources *resources) {
    resources->arcade_pal = load_file("GAME_DIR/AR1/STA/ARCADE.PAL");

    resources->room_roe   = load_file("GAME_DIR/AR1/MAP/ROOM.ROE");
    resources->buffer1    = load_file("GAME_DIR/AR1/STA/BUFFER1.MAT");
    resources->buffer2    = load_file("GAME_DIR/AR1/STA/BUFFER2.MAT");
    resources->buffer3    = load_file("GAME_DIR/AR1/STA/BUFFER3.MAT");
    resources->buffer4    = load_file("GAME_DIR/AR1/STA/BUFFER4.MAT");
    resources->buffer5    = load_file("GAME_DIR/AR1/STA/BUFFER5.MAT");
    resources->buffer6    = load_file("GAME_DIR/AR1/STA/BUFFER6.MAT");
    resources->bufferA    = load_file("GAME_DIR/AR1/STA/BUFFERA.MAT");
    resources->bufferB    = load_file("GAME_DIR/AR1/STA/BUFFERB.MAT");

    resources->status_ele = load_file("GAME_DIR/AR1/UCC/UCCI0.ELE");
    resources->numeri_ele = load_file("GAME_DIR/AR1/UCC/UCCI1.ELE");
    resources->k_ele      = load_file("GAME_DIR/AR1/IMG/K.ELE");
    resources->tr_ele     = load_file("GAME_DIR/AR1/IMG/TR.ELE");
    resources->ucci0_ele  = load_file("GAME_DIR/AR1/IMG/NUMERI.ELE");
    resources->ucci1_ele  = load_file("GAME_DIR/AR1/IMG/STATUS.ELE");

    resources->logi_tab    = load_file("GAME_DIR/AR1/FIL/LOGITAB.TAB");
    resources->frames_tab  = load_file("GAME_DIR/AR1/FIL/FRAMES.TAB");
    resources->animjoy_tab = load_file("GAME_DIR/AR1/FIL/ANIMJOY.TAB");
    resources->animofs_tab = load_file("GAME_DIR/AR1/FIL/ANIMOFS.TAB");

    resources->dsp         = load_file("GAME_DIR/AR1/FIL/DSP");
    resources->usc         = load_file("GAME_DIR/AR1/FIL/USC");
    resources->prt         = load_file("GAME_DIR/AR1/FIL/PRT");
}

void palette_load_from_file(tr_palette *palette, tr_palette_file *file) {
    for (int i = file->start; i < file->count; i++) {
        palette->color[i] = 0xff000000 |
            (file->palette[i - file->start].r <<  2) |
            (file->palette[i - file->start].g << 10) |
            (file->palette[i - file->start].b << 18);
    }
}

void palette_init(tr_palette *palette, tr_resources *resources) {
    memset(palette, 0, sizeof(tr_palette));

    palette_load_from_file(palette, resources->arcade_pal);
}

void tilesets_init(tr_tilesets *tilesets, tr_resources *resources) {
    memset(tilesets, 0, sizeof(tr_tilesets));

    tilesets->sets[0x1] = resources->buffer1;
    tilesets->sets[0x2] = resources->buffer2;
    tilesets->sets[0x3] = resources->buffer3;
    tilesets->sets[0x4] = resources->buffer4;
    tilesets->sets[0x5] = resources->buffer5;
    tilesets->sets[0x6] = resources->buffer6;
    tilesets->sets[0xA] = resources->bufferA;
    tilesets->sets[0xB] = resources->bufferB;
}

#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)

uint8_t screen_to_tile_x(int screen_x) {
    int x = screen_x / TILE_WIDTH;
    return MIN(MAX(x, 0), 19);
}

uint8_t screen_to_tile_y(int screen_y) {
    int y = screen_y / TILE_HEIGHT;
    return MIN(MAX(y, 0), 20);
}

uint8_t room_get_mat_file(uint8_t *room_file, int room, int pos) {
    int offset = (room * 0x04f4) + 0x0004 + (pos * 2) + 1;
    return *(room_file + offset);
}

uint16_t room_get_tile_id(uint8_t *room_file, int room, int pos) {
    int offset = (room * 0x04f4) + 0x0024 + (pos * 2);
    return from_big_endian(*(uint16_t *)(room_file + offset));
}

uint16_t room_get_tile_type(uint8_t *room_file, int room, int pos) {
    int offset = (room * 0x04f4) + 0x0364 + pos;
    return *(room_file + offset);
}

uint16_t room_get_tile_type_xy(uint8_t *room_file, int room, int x, int y) {
    int pos = (screen_to_tile_y(y) * GAME_TILES_WIDTH) + screen_to_tile_x(x);
    return room_get_tile_type(room_file, room, pos);
}

void render_background_tile(int tile_x,
                            int tile_y,
                            uint16_t tile_info,
                            uint8_t *src,
                            uint8_t *dst)
{
    int  tile_i = tile_info & 0x01ff;
    bool flip   = !!(tile_info & 0x0200);

    int tile_px_x = tile_x * TILE_WIDTH;
    int tile_px_y = tile_y * TILE_HEIGHT;

    int stride = TILE_WIDTH * GAME_TILES_WIDTH;

    int src_offset = tile_i * (TILE_WIDTH * TILE_HEIGHT);

    int x, y;

    src += src_offset;

    for (y = 0; y < TILE_HEIGHT; y++) {
        for (x = 0; x < TILE_WIDTH; x++) {
            int px = tile_px_x + x;
            int py = tile_px_y + y;
            int off = py * stride + px;
            int sx = !flip ? x : TILE_WIDTH - x - 1;
            int soff = y * TILE_WIDTH + sx;

            dst[off] = src[soff];
        }
    }
}

void render_background_layer(uint8_t *room_file, int room, tr_tilesets *sets, uint8_t *dst) {
    for (int i = 0; i < GAME_TILES_WIDTH * GAME_TILES_HEIGHT; i++) {
        int tile_x = i % 0x14;
        int tile_y = i / 0x14;

        int tile_id = room_get_tile_id(room_file, room, i);
        int file_id = ((tile_id & 0xf000) >> 12);
        int set_id = room_get_mat_file(room_file, room, file_id);

        if (set_id > 0xc) continue;

        uint8_t *src = sets->sets[set_id];
        if (!src) continue;

        render_background_tile(tile_x, tile_y, tile_id, src, dst);
    }
}

uint16_t read16_unaligned(void *x) {
    uint8_t *b = (uint8_t *)x;
    uint8_t l = *(b + 0);
    uint8_t h = *(b + 1);
    return (h << 8) + l;
}

uint32_t read32_unaligned(void *x) {
    uint16_t *b = (uint16_t *)x;
    uint16_t l = read16_unaligned((uint16_t *)(b + 0));
    uint16_t h = read16_unaligned((uint16_t *)(b + 1));
    return (h << 16) + l;
}

void tr_graphics_init(tr_graphics *graphics, uint8_t *ele_file) {
    graphics->count = (*(uint16_t *)ele_file);
    graphics->items = calloc(graphics->count, sizeof(tr_image_8bpp));

    for (int i = 0; i < graphics->count; i++)
    {
        tr_image_8bpp *dst_item = &graphics->items[i];

        int           offset    = (i * 4) + 2;
        uint8_t       *src_offset = ele_file + offset;
        uint8_t       *src_item = ele_file + 2 + read32_unaligned(src_offset);

        dst_item->width  = read16_unaligned(src_item + 0 * 2);
        dst_item->height = read16_unaligned(src_item + 1 * 2);

        int size = dst_item->width * dst_item->height;
        dst_item->pixels = malloc(size);
        dst_item->mask   = malloc(size);

        memset(dst_item->pixels, 0, size);
        memset(dst_item->mask,   0, size);

        {
            uint8_t *src = src_item + 5;
            uint8_t *dst = dst_item->pixels;
            uint8_t *msk = dst_item->mask;

            int x_left = dst_item->width;

            int consecutive_ffs = 0;

            while (1) {
                uint8_t skip = *src++;

                if (skip != 0xff) {
                    consecutive_ffs = 0;
                    dst += skip;
                    msk += skip;

                    x_left -= skip;

                    uint8_t count = *src++;
                    if (count != 0xff) {
                        consecutive_ffs = 0;

                        for (int i = 0; i < count / 2; i++) {
                            uint8_t colors = *src++;
                            uint8_t color1 = ((colors & 0x0f)     );
                            uint8_t color2 = ((colors & 0xf0) >> 4);

                            *dst++ = color1; *msk++ = 1;
                            *dst++ = color2; *msk++ = 1;
                            x_left -= 2;
                        }

                        if (count & 1) {
                            uint color = *src++;
                            uint color1 = (color & 0x0f);

                            *dst++ = color1; *msk++ = 1;

                            x_left--;
                        }
                    }
                    else {
                        consecutive_ffs++;
                        if (consecutive_ffs == 3) {
                            break;
                        }
                    }
                }
                else {
                    dst += x_left; msk += x_left;

                    x_left = dst_item->width;

                    consecutive_ffs++;
                    if (consecutive_ffs == 3) {
                        break;
                    }
                }
            }
        }
    }
}

void DrawTextureScaled(Texture texture, int x, int y, int width, int height, bool flip) {
    Rectangle sourceRect = {
        .x = !flip ? 0 : width,
        .y = 0,
        .width = width * (!flip ? 1 : -1),
        .height = height
    };

    Rectangle destRect = {
        .x = x * GAME_SIZE_SCALE,
        .y = y * GAME_SIZE_SCALE,
        .width = width  * GAME_SIZE_SCALE,
        .height = height * GAME_SIZE_SCALE
    };

    DrawTexturePro(texture, sourceRect, destRect, (Vector2) { .x  = 0, .y = 0 }, 0, WHITE);
}

void tr_graphics_to_textures(ray_textures *texts,
                             const tr_graphics *tr,
                             const tr_palette *palette,
                             int col)
{
    texts->count    = tr->count;
    texts->textures = calloc(texts->count, sizeof(Texture2D));
    texts->data     = calloc(texts->count, sizeof(uint32_t*));

    for (int image = 0; image < tr->count; image++) {
        tr_image_8bpp *img = &tr->items[image];
        texts->data[image] = malloc(img->width * img->height * sizeof(uint32_t));

        for (int pix = 0; pix < img->width * img->height; pix++) {
            uint8_t color = img->pixels[pix];
            uint8_t mask  = img->mask  [pix];

            texts->data[image][pix] = mask == 0 ? 0 : palette->color[color + col];
        }

        texts->textures[image] = LoadTextureFromImage((Image) {
            .data = texts->data[image],
            .width = img->width,
            .height = img->height,
            .format = UNCOMPRESSED_R8G8B8A8,
            .mipmaps = 1,
        });

        SetTextureFilter(texts->textures[image], FILTER_POINT);
    }
}

static void DrawTileTypes(const tr_resources *resources, int the_room) {
    for (int t_y = 0; t_y < GAME_TILES_HEIGHT; t_y++) {
        for (int t_x = 0; t_x < GAME_TILES_WIDTH; t_x++) {
            int x = GAME_SIZE_SCALE * t_x * TILE_WIDTH;
            int y = GAME_SIZE_SCALE * t_y * TILE_HEIGHT;

            uint8_t type = room_get_tile_type(resources->room_roe,
                                              the_room,
                                              t_y * GAME_TILES_WIDTH + t_x);

            char str[0x100];
            sprintf(str, "%2x", type);

            if (type != 0)
                DrawText(str, x, y, 20, GREEN);
        }
    }
}

char tr_keys_to_direction(bool down,
                          bool up,
                          bool left,
                          bool right,
                          bool fire)
{
    static char dpad_to_direction[0x10] = {
        0, 3, 7, 0, 1, 2, 8, 1, 5, 4, 6, 5, 0, 3, 4, 0
    };

    char dpad =
        (!!down  << 3) |
        (!!up    << 2) |
        (!!left  << 1) |
        (!!right << 0);

    return dpad_to_direction[dpad] + (fire ? 9 : 0);
}

bool set_is_member(char to_check, uint8_t *set) {
    char bit  = 1 << (to_check & 7);
    char byte = set[to_check / 8];

    return !!(byte & bit);
}

bool logi_tab_contains(uint8_t *logitab, int thing, int logi_tab_index) {
    uint16_t offset;
    uint8_t  *data;
    bool     retval = false;

    thing &= 0xff;
    logi_tab_index &= 0xff;

    offset = read16_unaligned(logitab + logi_tab_index * 2);
    offset = from_big_endian(offset);

    data = (logitab + offset);

    while (*data != 0xff) {
        if (*data == thing) {
            retval = true;
            break;
        }

        data++;
    }

    return retval;
}


void offset_from_ani(tr_pupo *pupo, tr_resources *resources) {
    uint16_t *offs = (uint16_t *)resources->animofs_tab;
    uint16_t  o1   = from_big_endian(offs[0]) + pupo->ani;
    uint16_t  o2   = from_big_endian(offs[1]) + pupo->ani;
    uint16_t  o3   = from_big_endian(offs[2]) + pupo->ani;
    uint16_t  o4   = from_big_endian(offs[3]) + pupo->ani;

    pupo->new_x = (
       pupo->x +
       *(int8_t *)(resources->animofs_tab + o1) +
       *(int8_t *)(resources->animofs_tab + o3)
    );

    pupo->new_y = (
       pupo->y +
       *(int8_t *)(resources->animofs_tab + o2) +
       *(int8_t *)(resources->animofs_tab + o4)
    );
}

void change_room(tr_pupo *pupo, tr_resources *resources, int room_to_change) {
    char previous_room = pupo->current_room;

    // reset_clicked_button();

    pupo->new_x = pupo->x;
    pupo->new_y = pupo->y;

//    previous_room = *no_previous_room ? 0 : *current_room_number;
//    *wanted_room = room_to_change;

//    if (!*need_to_save_arc_lzr && !*maybe_exit_related) {
//        gsa_and_exit_room(*wanted_room);
//    }

    pupo->new_x = pupo->x;
    pupo->new_y = pupo->y;


    pupo->current_room = room_to_change;
//    if (*vita >= 0x800) {
//        if (!*no_previous_room) {
//            room_to_change = *wanted_room;
//            get_room_from_files(*wanted_room);
//            calls_funcptr_1();
//        }
//    }
//    else
    {
//        room_to_change = *wanted_room;
//        get_room_from_files(*wanted_room);

        if (pupo->read_from_usc) {
            uint8_t *current_usc = resources->usc;

            while (1) {
                uint8_t room_from = *(current_usc + 0);
                uint8_t room_to   = *(current_usc + 1);

                if (room_from == 0xff && room_to == 0xff) {
                    break;
                }

                if ((room_from == previous_room) &&
                    (room_to == room_to_change))
                {
                    int y_from = from_big_endian(read16_unaligned(current_usc + 4));
                    int y_to   = from_big_endian(read16_unaligned(current_usc + 2));

                    if (y_from == pupo->new_x) {
                        pupo->new_y = y_to;
                        pupo->y     = y_to;
                        break;
                    }
                }

                current_usc += 6;
            }
        }
        else {
            /* read from prt */
        }

        pupo->to_set_x = pupo->new_x;
        pupo->to_set_y = pupo->new_y;

        // do_tiletype_actions_inner(0xffff);
    }

    // calls_funcptr_1();

    // *pupo_tile_top    = get_tile_type(*pupo_new_x, *pupo_new_y     );
    // *pupo_tile_bottom = get_tile_type(*pupo_new_x, *pupo_new_y + 10);

    // if (!*read_from_usc) {
    // }

    pupo->read_from_usc = false;

    // *no_previous_room = 0;

    // reset_array_bobs();
    // check_and_load_ucci();
    // reset_background_ani();

    // if (*vita <= 0x800) {
    //     animate_and_render_background();
    //     draw_punti_faccia();
    //     draw_stars();
    // }

    // copy_bg_to_vga();

    // (*swivar_block_1)[*current_room_number + 0x14] = 1;

    // if (!*should_stop_gameloop && !*disable_pupo_anim)
    //     maybe_fade(1);
}


void change_at_screen(tr_pupo *pupo, tr_resources *resources) {
    static uint8_t stru_121f0[] = {
        0x0E, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x1C,
        0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    char direction = pupo->ani >= 0x35;

    int pupo_real_x = pupo->x + pupo->x_delta;

    if ((pupo_real_x > 319 && direction) ||
        (pupo_real_x < 0 && !direction))
    {
        if (!set_is_member(pupo->ani, stru_121f0)) {
            pupo->read_from_usc = true;

            pupo->new_x = pupo->x > 0 ? pupo->x - 320 : pupo->x + 320;
            pupo->x = pupo_real_x > 0 ? 312 : 8;


            {
                int y = pupo->y - 0xa;
                char new_room;
                char tile_type;

                do {

                    new_room = room_get_tile_type_xy(resources->room_roe, pupo->current_room, pupo->x, y);

                    y += 0xa;

                    tile_type = room_get_tile_type_xy(resources->room_roe, pupo->current_room, pupo->x, y);

                    y += 0xa;

                    if (logi_tab_contains(resources->logi_tab, tile_type, 0x25))
                        break;
                } while (y <= 0xc7);

                pupo->x = pupo->new_x;
                change_room(pupo, resources, new_room);
            }
        }
    }
}

void update_pupo(tr_pupo *pupo, tr_resources *resources, uint8_t direction) {
    char enemy_hit = 0;
    char get_new_frame = 0;

    do {
        if (!enemy_hit)
            get_new_frame = 0;

        if (pupo->get_new_ani && !enemy_hit)
        {
            pupo->ani = resources->animjoy_tab[pupo->ani * 18 + direction];

            pupo->get_new_ani = 0;
            get_new_frame = 1;

            // *pupo_tile_top    = get_tile_type(*pupo_x, *pupo_y     );
            // *pupo_tile_bottom = get_tile_type(*pupo_x, *pupo_y + 10);
            // controlla_sotto_piedi(ani, top, bot, x, y, nx, ny);

            /*
            if (*gun_bool == 0) {
                int a = *pupo_current_ani;

                *pupo_current_ani =
                    (a == 0x47) ? 0x48:
                    (a == 0x12) ? 0x13:
                    a;
            }
             */

//            int a = pupo->ani;
//
//            do {
                offset_from_ani(pupo, resources);

//                pupo->ani = cosa_ho_di_fronte(&__ani, X, Y, NX, NY);
//                a = pupo->ani;
//            } while (pupo->ani != a);

        }

        if (pupo->disable_ani) {
            get_new_frame = 1;
            pupo->disable_ani = 0;
        }

        // gsa_and_exit();

        if (pupo->countdown == 0) {
            if (get_new_frame) {
                //*byte_1f4dc = 0;

                uint8_t *off = resources->frames_tab + (pupo->ani * 2);
                pupo->pupo_offset = from_big_endian(*(uint16_t *)(off));

                uint16_t *offs = (uint16_t *)resources->animofs_tab;
                uint16_t  o1   = from_big_endian(offs[0]) + pupo->ani;
                uint16_t  o2   = from_big_endian(offs[1]) + pupo->ani;

                pupo->x += *(int8_t *)(resources->animofs_tab + o1);
                pupo->y += *(int8_t *)(resources->animofs_tab + o2);
            }

            uint8_t *frame_info = resources->frames_tab + pupo->pupo_offset;

            uint8_t frame = frame_info[0];
            uint8_t time  = frame_info[1];

            if ((frame + time) != 0) {
                uint8_t x_delta = frame_info[2];
                uint8_t y_delta = frame_info[3];
                uint8_t flip    = frame_info[4];

                pupo->pupo_offset += 5; /* sizeof(frame_info_t) */
                pupo->ele_idx  = frame;
                pupo->countdown = (time + 3) / 4;
                pupo->x_delta   = x_delta;
                pupo->y_delta   = y_delta;
                pupo->flip      = flip;
            }
            else {
                pupo->get_new_ani = true;
            }

            if (pupo->get_new_ani) {
                uint16_t *offs = (uint16_t *)resources->animofs_tab;
                uint16_t  o3   = from_big_endian(offs[2]) + pupo->ani;
                uint16_t  o4   = from_big_endian(offs[3]) + pupo->ani;

                pupo->x += *(int8_t *)(resources->animofs_tab + o3);
                pupo->y += *(int8_t *)(resources->animofs_tab + o4);

                if ((pupo->to_set_x < 0) ||
                    (pupo->to_set_x > 319))
                {
                    pupo->to_set_x = pupo->x;
                    pupo->to_set_x = pupo->y;
                }
            }

            // update_pupo_4();
            // sub_1243d();

        }
    } while (pupo->get_new_ani);

    change_at_screen(pupo, resources);

    // sub_122f1();

    if (!pupo->disable_ani) {
        pupo->countdown--;
    }
}


void ray_bg_renderer_init(ray_bg_renderer *bg, tr_resources *resources, tr_tilesets *tilesets, tr_palette *palette) {
    bg->resources = resources;
    bg->palette = palette;
    bg->tilesets = tilesets;

    bg->texture = LoadTextureFromImage((Image) {
        .data = &bg->data,
        .width = GAME_SIZE_WIDTH,
        .height = GAME_SIZE_HEIGHT,
        .format = UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1,
    });

    SetTextureFilter(bg->texture, FILTER_POINT);
}

void ray_bg_render_room(ray_bg_renderer *bg, int room_nr) {
    render_background_layer(bg->resources->room_roe, room_nr, bg->tilesets, bg->background);

    for (int i = 0; i < GAME_SIZE_WIDTH * GAME_SIZE_HEIGHT; i++) {
        bg->data[i] = bg->palette->color[bg->background[i]];
    }

    UpdateTexture(bg->texture, bg->data);
}

int main() {
    InitWindow(GAME_SIZE_WIDTH  * GAME_SIZE_SCALE,
               GAME_SIZE_HEIGHT * GAME_SIZE_SCALE,
               "Corridori");

    SetTargetFPS(15);

    tr_resources resources;
    tr_palette   palette;
    tr_tilesets  tilesets;

    tr_graphics status_ele;
    tr_graphics numeri_ele;
    tr_graphics k_ele;
    tr_graphics tr_ele;
    tr_graphics ucci0_ele;
    tr_graphics ucci1_ele;

    resources_load(&resources);

    palette_init(&palette, &resources);
    tilesets_init(&tilesets, &resources);

    tr_graphics_init(&status_ele, resources.status_ele);
    tr_graphics_init(&numeri_ele, resources.numeri_ele);
    tr_graphics_init(&k_ele, resources.k_ele);
    tr_graphics_init(&tr_ele, resources.tr_ele);
    tr_graphics_init(&ucci0_ele, resources.ucci0_ele);
    tr_graphics_init(&ucci1_ele, resources.ucci1_ele);

    ray_textures status_tex;
    ray_textures numeri_tex;
    ray_textures k_tex;
    ray_textures tr_tex;
    ray_textures ucci0_tex;
    ray_textures ucci1_tex;

    tr_graphics_to_textures(&status_tex, &status_ele, &palette, 0xc1);
    tr_graphics_to_textures(&numeri_tex, &numeri_ele, &palette, 0xc1);
    tr_graphics_to_textures(&k_tex,      &k_ele,      &palette, 0xc1);
    tr_graphics_to_textures(&tr_tex,     &tr_ele,     &palette, 0xc1);
    tr_graphics_to_textures(&ucci0_tex,  &ucci0_ele,  &palette, 0xc1);
    tr_graphics_to_textures(&ucci1_tex,  &ucci1_ele,  &palette, 0xc1);

    ray_bg_renderer bg_renderer;

    ray_bg_renderer_init(&bg_renderer,
                         &resources,
                         &tilesets,
                         &palette);

    bool show_types = false;

    tr_pupo pupo;
    memset(&pupo, 0, sizeof(pupo));

    pupo.x = 0x08;
    pupo.y = 0xa0;
    pupo.ani = 0x03;
    pupo.ele_idx = 0x00;
    pupo.flip = 0;
    pupo.countdown = 0;
    pupo.frame_nr = 0;
    pupo.get_new_ani = 1;

    int rendered_room = 0;
    ray_bg_render_room(&bg_renderer, 0);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyPressed(KEY_M)) {
            show_types = !show_types;
        }

        char direction = tr_keys_to_direction(
          IsKeyDown(KEY_DOWN),
          IsKeyDown(KEY_UP),
          IsKeyDown(KEY_LEFT),
          IsKeyDown(KEY_RIGHT),
          IsKeyDown(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)
        );

        update_pupo(&pupo, &resources, direction);

        if (rendered_room != pupo.current_room) {
            ray_bg_render_room(&bg_renderer, pupo.current_room);
            rendered_room = pupo.current_room;
        }

        DrawTextureScaled(bg_renderer.texture, 0, 0, GAME_SIZE_WIDTH, GAME_SIZE_HEIGHT, false);

        {
            int x = pupo.x + pupo.x_delta;
            int y = pupo.y + pupo.y_delta;
            int w = tr_tex.textures[pupo.ele_idx].width;
            int h = tr_tex.textures[pupo.ele_idx].height;

            x -= w / 2;
            y -= h;

            DrawTextureScaled(tr_tex.textures[pupo.ele_idx], x, y, w, h, pupo.flip);
        }

        char suca[0x100];

        if (show_types)
            DrawTileTypes(&resources, 0);

        sprintf(suca, "dir:   %2x", direction);
        DrawText(suca, 20,  0, 20, GREEN);
        sprintf(suca, "ani:   %2x", pupo.ani);
        DrawText(suca, 20, 20, 20, GREEN);
        sprintf(suca, "frame: %2x", pupo.ele_idx);
        DrawText(suca, 20, 40, 20, GREEN);
        sprintf(suca, "ctd:   %2x", pupo.countdown);
        DrawText(suca, 20, 60, 20, GREEN);
        sprintf(suca, "pupo:   %4d %4d", pupo.x, pupo.y);
        DrawText(suca, 20, 80, 20, GREEN);
        sprintf(suca, "offs:   %4x", pupo.pupo_offset);
        DrawText(suca, 20, 100, 20, GREEN);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
