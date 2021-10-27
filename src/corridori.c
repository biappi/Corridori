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
    uint8_t *sostani_tab;

    uint8_t *dsp;
    uint8_t *usc;
    uint8_t *prt;
    uint8_t *swi;
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
    int  wanted_room;
    bool read_from_usc;

    uint16_t tile_top;
    uint16_t tile_bottom;

    uint8_t  byte_1f4dc;
    uint8_t  byte_1f4e6;
    uint8_t  counter_caduta;
    uint16_t vita;

    uint16_t palette_mangling;

    bool swivar2[0x40];

    void *swi_elements[0x40];
}  tr_pupo;

typedef struct {
    int frame;
    int countdown;
} tr_bg;

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
    resources->sostani_tab = load_file("GAME_DIR/AR1/FIL/SOSTANI.TAB");

    resources->dsp         = load_file("GAME_DIR/AR1/FIL/DSP");
    resources->usc         = load_file("GAME_DIR/AR1/FIL/USC");
    resources->prt         = load_file("GAME_DIR/AR1/FIL/PRT");
    resources->swi         = load_file("GAME_DIR/AR1/FIL/SWI");
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

void swi_elements_init(tr_pupo *pupo, tr_resources *resources) {
    uint8_t *swi = resources->swi;

    for (int i = 0; i < 0x40; i++) {
        uint16_t type  = from_big_endian(read16_unaligned(swi + 0)); (type = type);
        uint16_t count = from_big_endian(read16_unaligned(swi + 2));

        pupo->swi_elements[i] = swi;
        swi += count;
    }
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

void room_set_tile_id_tile_x_tile_y(uint8_t *room_file, int room, int x, int y, uint16_t type) {
    int pos = (y * GAME_TILES_WIDTH) + x;
    int offset = (room * 0x04f4) + 0x0024 + (pos * 2);
    uint16_t *ptr = (uint16_t *)(room_file + offset);
    *ptr = from_big_endian(type);
}

uint8_t room_get_tile_type(uint8_t *room_file, int room, int pos) {
    int offset = (room * 0x04f4) + 0x0364 + pos;
    return *(room_file + offset);
}

uint8_t room_get_tile_type_xy(uint8_t *room_file, int room, int x, int y) {
    int pos = (screen_to_tile_y(y) * GAME_TILES_WIDTH) + screen_to_tile_x(x);
    return room_get_tile_type(room_file, room, pos);
}

void room_set_tile_type_tile_x_tile_y(uint8_t *room_file, int room, int x, int y, uint8_t type) {
    int pos = y * GAME_TILES_WIDTH + x;
    int offset = (room * 0x04f4) + 0x0364 + pos;
    *(room_file + offset) = type;
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

uint16_t adjust_bg_tile_animation(uint16_t tile, int frame) {
    if ((tile & 0xf000) != 0x9000)
        return tile;

    if (tile & 0x0800) {
        frame = 3;
    }
    else {
        if ((tile & 0x0400) && (frame == 3)) {
            tile = tile | 0x0800;
        }
    }

    return (tile & 0xfffc) | (frame & 0x0003);
}

void render_background_layer(uint8_t *room_file, int room, tr_tilesets *sets, uint8_t *dst, int frame) {
    for (int i = 0; i < GAME_TILES_WIDTH * GAME_TILES_HEIGHT; i++) {
        int tile_x = i % 0x14;
        int tile_y = i / 0x14;

        int tile_id = room_get_tile_id(room_file, room, i);
        tile_id = adjust_bg_tile_animation(tile_id, frame);

        int file_id = ((tile_id & 0xf000) >> 12);
        int set_id = room_get_mat_file(room_file, room, file_id);

        if (set_id > 0xc) continue;

        uint8_t *src = sets->sets[set_id];
        if (!src) continue;

        render_background_tile(tile_x, tile_y, tile_id, src, dst);
    }
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

void tiletype_action_8(tr_pupo *pupo, tr_resources *resources, void *swi_ptr, uint16_t *idx, uint16_t *top) {
    uint8_t *swi_element = (uint8_t *)swi_ptr;

    uint16_t count = read16_unaligned(swi_element + 0);
    count = from_big_endian(count);

    swi_element = swi_element + 2;

    for (int i = 0; i < count; i++) {
        uint16_t xy     = from_big_endian(read16_unaligned(swi_element + 0));
        uint16_t room   = from_big_endian(read16_unaligned(swi_element + 2));
        uint16_t tileid = from_big_endian(read16_unaligned(swi_element + 4));

        if (room == pupo->current_room) {
            unsigned int x = xy % 20;
            unsigned int y = xy / 20;

            room_set_tile_id_tile_x_tile_y(resources->room_roe,
                                           pupo->current_room,
                                           x,
                                           y,
                                           tileid);
        }

        swi_element = swi_element + 6;
    }

    count = read16_unaligned(swi_element + 0);
    count = from_big_endian(count);

    swi_element = swi_element + 2;

    for (int i = 0; i < count; i++) {
        uint16_t xy       = from_big_endian(read16_unaligned(swi_element + 0));
        uint8_t  room     = *(swi_element + 2);
        uint8_t  tiletype = *(swi_element + 3);

        if (room == pupo->current_room) {
            unsigned int x = xy % 20;
            unsigned int y = xy / 20;

            printf(" changing tile type: xy %4x room %4x tileid %4x -- %2x %2x\n", xy, room, tiletype, x, y);

            room_set_tile_type_tile_x_tile_y(resources->room_roe,
                                             pupo->current_room,
                                             x,
                                             y,
                                             tiletype);
        }

        swi_element = swi_element + 4;
    }
}

void do_tiletype_actions_inner(tr_pupo *pupo, tr_resources *resources, uint16_t boh) {
    boh = boh & 0xff;

    uint16_t idx = boh == 0xff ? 0    : boh;
    uint16_t top = boh == 0xff ? 0x3f : boh;

    for (uint16_t i = idx; i <= top; i++) {
        if (!pupo->swivar2[i])
            continue;

        void     *ptr1 = pupo->swi_elements[i];
        uint8_t  *ptr2 = (((uint8_t *)ptr1) + 4);

        uint16_t  type = read16_unaligned(ptr1);
        type = from_big_endian(type);

        if (type != 0) {
//            *background_ani_frame = 0;

            if (type == 8) {
                tiletype_action_8(pupo, resources, ptr2, &i, &top);
            }
            else {
                printf("%02x No tiletype action yet", type);
            }
        }
    }
}

void do_tiletype_actions(tr_pupo *pupo, tr_resources *resources, char boh) {
    if (boh > 0x3f)
        return;

    if (pupo->swivar2[boh] == false) {
        pupo->swivar2[boh] = true;

        do_tiletype_actions_inner(pupo, resources, boh);
    }
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

        do_tiletype_actions_inner(pupo, resources, 0xffff);
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

void cosa_ho_di_fronte(tr_pupo *pupo, tr_resources* resources, uint8_t *ani, int x, int y, int nx, int ny) {
    uint8_t  *sostani = resources->sostani_tab;
    uint16_t  offset  = from_big_endian(read16_unaligned(sostani + 2));

    uint8_t  *sostani_item = sostani + offset;

    uint8_t new_ani = *ani;

    uint8_t ani_from_file;
    uint8_t logitab_idx;
    char x_offset;

    bool not_found = false;
    bool changed   = false;

    uint8_t oldani;

    do {
        changed = false;
        not_found = false;

        while (1) {
            oldani = *(sostani_item);

            if (oldani == 0xff) {
                not_found = true;
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

        int xoff = nx - x + x_offset;
        xoff = xoff >  0x40 ? xoff + 0x10 : xoff;
        xoff = xoff < -0x40 ? xoff - 0x10 : xoff;

        int incr = xoff < 0 ? 0x10 : -0x10;

        while (xoff != 0) {
            uint16_t tile_type = room_get_tile_type_xy(resources->room_roe, pupo->current_room, x + xoff, y);

            if (logi_tab_contains(resources->logi_tab, tile_type, logitab_idx)) {
                new_ani = ani_from_file;
                changed = 1;
                break;
            }

            xoff += incr;
        }
    } while (changed);

    *ani = new_ani;
}

void cambia_il_salto(tr_resources *resources, uint8_t *ani, int top) {
    int offset = *(uint16_t *)(resources->sostani_tab);
    uint8_t *sostani_item = resources->sostani_tab + from_big_endian(offset);

    uint8_t oldani;

    // printf("cambia\n");

    while (1) {
        oldani = *(sostani_item);

        if (oldani == 0xff) {
            break;
        }

        if (oldani == *ani)
        {
            uint8_t logitab_index = *(sostani_item + 1);
            uint8_t new_ani       = *(sostani_item + 2);

            // printf("  top: %2x oldani: %2x newani: %2x logiidx: %2x\n", top, oldani, new_ani, logitab_index);

            if (logi_tab_contains(resources->logi_tab, top, logitab_index))
                *ani = new_ani;

        }

        sostani_item += 3;
    }

    // printf("\n");
}

void controlla_sotto_piedi(
    tr_pupo *pupo,
    tr_resources *resources,
    uint8_t  *ani_ptr,
    uint16_t top,
    uint16_t bottom,
    uint16_t x,
    uint16_t y,
    uint16_t new_x,
    uint16_t new_y
) {
    bool direction = pupo->ani >= 0x35;

    int  var2 = 0;
    char tile_type;

#if 0
    if (*byte_1f4e6 == 0 && top == 0xfc) {
        /* loc_11e91 */
        vga_dump(10, 10, "NOPE 1"); while (1);
        return;
    }
#endif

    if (logi_tab_contains(resources->logi_tab, bottom, 0x25)) {
        /* loc_11ea8 */
        pupo->byte_1f4e6 = pupo->byte_1f4e6 - 1;

#if 0
        if ((pupo->byte_1f4e6 == 0) &&
            (*has_to_adjust_ani != 0))
        {
            void (far pascal *reset_pupo_caduta)() = MK_FP(seg002, 0x7b1);

            ds_trampoline_end();
            reset_pupo_caduta();
            ds_trampoline_start();
        }
        else
#endif
        {
            *ani_ptr = direction ? 0x67 : 0x32;
            pupo->counter_caduta = pupo->counter_caduta + 1;
        }

        return;
    }

#if 0
    if (pupo->byte_1f4e6 > 0) {
        /* loc_11ecf */
        vga_dump(10, 10, "NOPE 3"); while (1);
        return;
    }

    if (top == 9) {
        /* fai cade */
        vga_dump(10, 10, "NOPE 4"); while (1);
        return;
    }
#endif

    if (top == 0) {
        *ani_ptr = direction ? 0x67 : 0x32;
        pupo->counter_caduta = pupo->counter_caduta + 1;
        return;
    }

    if (pupo->counter_caduta > 0) {
        /* loc_11f3c */

        if (pupo->counter_caduta > 8) {
            *ani_ptr = direction ? 0x68 : 0x33;
        }

        pupo->counter_caduta = 0;
        return;
    }

#if 0
    if (pupo->byte_1f4e8 < 0) {
        pupo->byte_1f4e8 = pupo->byte_1f4e8 - 1;
        if (set_is_member(*ani, MK_FP(seg002, 0x0E79))) {
            /* set ani 38_3 */
            vga_dump(10, 10, "NOPE 99"); while (1);
            var2 = 1;
        }
    }

    if (!var2) {
        if (*byte_1f4e9 > 0) {
            *byte_1f4e9 = *byte_1f4e9 - 1;
        }
        if (set_is_member(*ani, MK_FP(seg002, 0x0e99))) {
            /* set ani 38_3 */
            vga_dump(10, 10, "NOPE 66"); while (1);
            var2 = 1;
        }
    }
#else
    var2 = 0;
#endif

    if (!var2) {
        cambia_il_salto(resources, ani_ptr, top);
    }

#if 0
    if ((*ani == 0x12) || (*ani == 0x13)) {
        /* sub_11a14 */
        vga_dump(10, 10, "NOPE 6"); while (1);
    }

    if ((*ani == 0x47) || (*ani == 0x48)) {
        /* sub_11a3d */
        vga_dump(10, 10, "NOPE 7"); while (1);
    }

    if (set_is_member(*ani, MK_FP(seg002, 0x0e79))) {
        /* loc_11ff6 */
        vga_dump(10, 10, "NOPE 8"); while (1);
        return;
    }

    if (set_is_member(*ani, MK_FP(seg002, 0x0e99))) {
        /* loc_12011 */
        vga_dump(10, 10, "NOPE 9"); while (1);
        return;
    }

    if (*ani == 0x14) {
        /* loc_12027 */
        vga_dump(10, 10, "NOPE 10"); while (1);
        return;
    }

    if (*ani == 0x49) {
        /* loc_12036 */
        vga_dump(10, 10, "NOPE 11"); while (1);
        return;
    }

    if (*ani == 0x8) {
        /* loc_12045 */
        vga_dump(10, 10, "NOPE 12"); while (1);
        return;
    }

    if (*ani == 0x9) {
        /* loc_12054 */
        vga_dump(10, 10, "NOPE 13"); while (1);
        return;
    }

    if (*ani == 0x3d) {
        /* loc_12063 */
        vga_dump(10, 10, "NOPE 14"); while (1);
        return;
    }

    if (*ani == 0x3e) {
        /* loc_12072 */
        vga_dump(10, 10, "NOPE 15"); while (1);
        return;
    }
#endif


    if (*ani_ptr == 0x10) {
        if (logi_tab_contains(resources->logi_tab, top, 0x26) ||
            logi_tab_contains(resources->logi_tab, top, 0x28))
         {
            if (new_x - 10 >= 0) {
                tile_type = room_get_tile_type_xy(resources->room_roe,
                                                  pupo->current_room,
                                                  new_x - 10,
                                                  new_y);

                if (tile_type == top) {
                    *ani_ptr = 0x1d;
                }
            }
        }

        return;
    }

    if (*ani_ptr == 0x45) {
        /* loc_12088 */
        if (logi_tab_contains(resources->logi_tab, top, 0x26) ||
            logi_tab_contains(resources->logi_tab, top, 0x28))
         {
            if ((new_x + 10) < 320) {
                tile_type = room_get_tile_type_xy(resources->room_roe,
                                                  pupo->current_room,
                                                  new_x + 10,
                                                  new_y);

                if (tile_type == top) {
                    *ani_ptr = 0x52;
                }
            }
        }

        return;
    }

    if ((*ani_ptr == 0x1f) ||
        (*ani_ptr == 0x20) ||
        (*ani_ptr == 0x54))
    {
        /* loc_12094 */

        /* eventually_change_room(); */
        if (logi_tab_contains(resources->logi_tab, top, 0x28)) {
            uint8_t the_ani = *ani_ptr;

            pupo->wanted_room = pupo->current_room;

            /*
            do_tiletype_actions(top_copy - 0xc0);
            rese();
            calls_funcptr_1();
            */

            if (pupo->wanted_room != pupo->current_room) {
                change_room(pupo, resources, pupo->wanted_room);
            }

            if (the_ani != *ani_ptr) {
                *ani_ptr = 0x22;
            }
        }
        else {
            if (top == 0xa8) {
                *ani_ptr = 0x20;
            }
            else if (top == 0xfe) {
                *ani_ptr = 0x54;
            }
            else {
                // *byte_1f4ec = 0;
            }
        }

        return;
    }

    if ((*ani_ptr == 0x21) ||
        (*ani_ptr == 0x55))
    {
        /* loc_120a3 */
        /* check_tile_switch_room(); */

        char pupo_tile = room_get_tile_type_xy(resources->room_roe,
                                               pupo->current_room,
                                               new_x,
                                               new_y);

        if (!logi_tab_contains(resources->logi_tab, pupo_tile, 0x25) ||
            !logi_tab_contains(resources->logi_tab, pupo_tile, 0x27))
        {
            tile_type = room_get_tile_type_xy(resources->room_roe,
                                              pupo->current_room,
                                              new_x,
                                              new_y - 0x0a);

            change_room(pupo, resources, tile_type);
        }

        return;
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

            pupo->tile_top = room_get_tile_type_xy(resources->room_roe,
                                                   pupo->current_room,
                                                   pupo->x,
                                                   pupo->y);

            pupo->tile_bottom = room_get_tile_type_xy(resources->room_roe,
                                                   pupo->current_room,
                                                   pupo->x,
                                                   pupo->y + 10);

            controlla_sotto_piedi(pupo,
                                  resources,
                                  &pupo->ani,
                                  pupo->tile_top,
                                  pupo->tile_bottom,
                                  pupo->x,
                                  pupo->y,
                                  pupo->new_x,
                                  pupo->new_y);

            /*
            if (*gun_bool == 0) {
                int a = *pupo_current_ani;

                *pupo_current_ani =
                    (a == 0x47) ? 0x48:
                    (a == 0x12) ? 0x13:
                    a;
            }
             */

            int a = pupo->ani;

            do {
                offset_from_ani(pupo, resources);

                cosa_ho_di_fronte(pupo,
                                  resources,
                                  &(pupo->ani),
                                  pupo->x,
                                  pupo->y,
                                  pupo->new_x,
                                  pupo->new_y);
                a = pupo->ani;
            } while (pupo->ani != a);
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

            pupo->byte_1f4dc = pupo->byte_1f4dc + 1;

            if ((pupo->tile_top & 0xf0) == 0xd0)
            {

                char type = (pupo->tile_top & 0x0f) + 0xc;

                do_tiletype_actions(pupo, resources, type);
                // reset_clicked_button();
                // calls_funcptr_1();

                {
                    pupo->new_x = pupo->x;
                    pupo->new_y = pupo->y;

                    pupo->tile_top    = room_get_tile_type_xy(resources->room_roe,
                                                              pupo->current_room,
                                                              pupo->x,
                                                              pupo->y);

                    pupo->tile_bottom = room_get_tile_type_xy(resources->room_roe,
                                                              pupo->current_room,
                                                              pupo->x,
                                                              pupo->y + 10);

                    // reset_array_bobs();
                    // reset_background_ani();

                    if (pupo->vita > 0x800) {
                        // animate_and_render_background();
                        // draw_stars();
                        // draw_punti_faccia();
                    }

                    {
                        // copy_bg_to_vga();

                        // select_pal(1, 0);
                        // install_ucci();

                        pupo->palette_mangling = 2;
                    }
                }
            }


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

void ray_bg_render_room(ray_bg_renderer *bg, int room_nr, int frame) {
    render_background_layer(bg->resources->room_roe, room_nr, bg->tilesets, bg->background, frame);

    for (int i = 0; i < GAME_SIZE_WIDTH * GAME_SIZE_HEIGHT; i++) {
        bg->data[i] = bg->palette->color[bg->background[i]];
    }

    UpdateTexture(bg->texture, bg->data);
}

bool bg_step(tr_bg *bg) {
    bool ret = false;

    bg->countdown = MAX(0, MIN(4, bg->countdown));
    bg->countdown--;

    if (bg->countdown <= 0) {
        bg->frame++;
        bg->countdown = 4;
        ret = true;
    }

    if (bg->frame >= 4) {
        bg->frame = 0;
        ret = true;
    }

    return  ret;
}

int main() {
    InitWindow(GAME_SIZE_WIDTH  * GAME_SIZE_SCALE,
               GAME_SIZE_HEIGHT * GAME_SIZE_SCALE,
               "Corridori");

    SetTargetFPS(20);

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

    tr_bg bg;

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

    swi_elements_init(&pupo, &resources);

    int rendered_room = 0;
    ray_bg_render_room(&bg_renderer, 0, 0);


    bool show_types  = true;
    bool show_anidbg = false;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyPressed(KEY_U)) {
            show_types = !show_types;
        }

        if (IsKeyPressed(KEY_P)) {
            show_anidbg = !show_anidbg;
        }

        char direction = tr_keys_to_direction(
          IsKeyDown(KEY_DOWN),
          IsKeyDown(KEY_UP),
          IsKeyDown(KEY_LEFT),
          IsKeyDown(KEY_RIGHT),
          IsKeyDown(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)
        );

        update_pupo(&pupo, &resources, direction);

        bool redraw_bg = bg_step(&bg);

        if ((rendered_room != pupo.current_room) || redraw_bg) {
            ray_bg_render_room(&bg_renderer, pupo.current_room, bg.frame);
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
            DrawTileTypes(&resources, pupo.current_room);

        if (show_anidbg) {
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
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
