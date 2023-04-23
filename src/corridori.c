#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "raylib.h"
#include "cimgui-raylib.h"

// #ifndef PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
// #define PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 UNCOMPRESSED_R8G8B8A8
// #endif

#define PACKED __attribute__((packed))

const int DEBUG_PANE_WIDTH = 300;
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
    uint16_t value;
} tr_tileid;

typedef struct {
    uint16_t value;
} tr_tiletype;

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
    uint8_t *pn;
    uint8_t *pu;
    uint8_t *ptx;

    uint8_t *texts_it;
} tr_resources;

typedef enum {
    tr_ele_status,
    tr_ele_numeri,
    tr_ele_k,
    tr_ele_tr,
    tr_ele_ucci0,
    tr_ele_ucci1,
    tr_ele_count,
} tr_ele_kind;

typedef struct PACKED {
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;

    uint8_t  ignored1;
    uint8_t  ignored2;
    uint8_t  vita;
    uint8_t  ignored3;

    uint8_t  boh1;
    uint8_t  boh2;
    uint8_t  bool1;
    uint8_t  maybe_item_type;
    uint16_t copy_x1;
    uint16_t copy_y1;

    uint8_t  sprite_nr;
    uint8_t  ignored4;
    uint8_t  ignored5;
    uint8_t  ignored6;
    uint8_t  wanted_ucci_nr;
    uint8_t  unk26;
    uint16_t unk27;
} tr_pu_item;

typedef struct {
    int16_t x;
    int16_t y;

    uint8_t  ani;
    uint8_t  frame_nr;

    bool     flip;
    int      countdown;

    // - //

    int8_t  x_delta;
    int8_t  y_delta;

    int16_t new_x;
    int16_t new_y;

    int16_t to_set_x;
    int16_t to_set_y;

    uint16_t pupo_offset;

    bool get_new_ani;
    bool disable_ani;

    // definitely not for her

    int  current_room;
    int  wanted_room;
    bool read_from_usc;

    tr_tiletype tile_top;
    tr_tiletype tile_bottom;

    uint8_t  byte_1f4dc;
    uint8_t  byte_1f4e6;
    uint8_t  byte_1f4e8;
    uint8_t  byte_1f4e9;

    uint8_t  counter_caduta;
    uint16_t vita;
    uint16_t score;
    uint8_t  faccia_countdown;
    uint8_t  punti_countdown;
    uint8_t  stars_countdown;
    uint8_t  stars_sprite_nr;
    uint8_t  gun_bool;
    uint8_t  colpi;

    uint16_t palette_mangling;

    uint32_t swivar1[0x400];
    bool     swivar2[0x400];

    void *swi_elements[0x40];

    tr_ele_kind current_ucci;
    uint8_t wanted_ucci_nr;
    uint8_t current_ucci_nr;

    bool    enemy_flip[2];
    bool    ucci_in_use[2];
    uint8_t byte_2197a[2];

}  tr_game;

// - //

typedef struct {
    enum {
        tr_resource_type_ele,
        tr_resource_type_ani,
    } type;

    union {
        struct {
            tr_ele_kind ele_kind;
            int ele_idx;
        } ele;

        struct {
            int ani_idx;
            int frame_idx;
        } ani;
    } res;
} tr_graphics_resource;

typedef struct {
    int frame;
    int countdown;
} tr_bg;

typedef enum {
    tr_render_command_graphics,
    tr_render_command_type_render_box_text,
    tr_render_command_arcade_background,
    tr_render_command_arcade_types,
} tr_render_command_type;

typedef struct {
    int x;
    int y;
    tr_graphics_resource res;
    bool flip;
    bool center_bottom_anchor;

    int palette_start; // still todo
    int color;         // still todo
} tr_render_graphics_command;

typedef struct {
    int x;
    int y;
    int ani_idx;
    int frame_idx;
} tr_render_ani_command;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    uint32_t color;
    const char *text;
    int slot;
} tr_render_box_text_command;

typedef struct {
    int x;
    int y;
    tr_ele_kind ele;
    int ele_idx;
    int flip;
    int palette_start;
    int color;
} tr_render_arcade_bob_command;

typedef struct {
    tr_render_command_type type;
    union {
        tr_render_graphics_command render_graphics;
        tr_render_box_text_command render_box_text;
    } command;
} tr_render_command;

typedef struct {
    tr_render_command commands[0x100];
    int count;
} tr_renderer;

void tr_renderer_clear(tr_renderer *renderer) {
    renderer->count = 0;
}

tr_render_command *tr_renderer_add_command(tr_renderer *renderer) {
    return &renderer->commands[renderer->count++];
}

// - //

typedef struct {
    int count;
    Texture *textures;
    uint32_t  **data;
} ray_textures;

typedef struct {
    uint32_t data[GAME_SIZE_WIDTH * GAME_SIZE_HEIGHT];
    uint8_t background[GAME_SIZE_WIDTH * GAME_SIZE_HEIGHT];

    tr_resources *resources;
    tr_palette   *palette;
    tr_tilesets  *tilesets;
    Texture      texture;
} ray_bg_renderer;

// - //

uint16_t from_big_endian(uint16_t x) {
    return ((x & 0x00ff) << 8) | (x >> 8);
}

uint16_t read16_unaligned(void *x) {
    uint8_t *b = (uint8_t *)x;
    uint16_t l = *(b + 0);
    uint16_t h = *(b + 1);
    return (h << 8) + l;
}

uint32_t read32_unaligned(void *x) {
    uint16_t *b = (uint16_t *)x;
    uint32_t l = read16_unaligned((uint16_t *)(b + 0));
    uint32_t h = read16_unaligned((uint16_t *)(b + 1));
    return (h << 16) + l;
}

void *load_file_return_length(const char *path, size_t *size) {
    FILE *file = fopen(path, "r");
    if (!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);

    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t *buf = malloc(*size);
    fread(buf, *size, 1, file);
    fclose(file);

    return buf;
}

void *load_file(const char *path) {
    size_t size;
    return load_file_return_length(path, &size);
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

    resources->status_ele = load_file("GAME_DIR/AR1/IMG/STATUS.ELE");
    resources->numeri_ele = load_file("GAME_DIR/AR1/IMG/NUMERI.ELE");
    resources->k_ele      = load_file("GAME_DIR/AR1/IMG/K.ELE");
    resources->tr_ele     = load_file("GAME_DIR/AR1/IMG/TR.ELE");
    resources->ucci0_ele  = load_file("GAME_DIR/AR1/UCC/UCCI0.ELE");
    resources->ucci1_ele  = load_file("GAME_DIR/AR1/UCC/UCCI1.ELE");

    resources->logi_tab    = load_file("GAME_DIR/AR1/FIL/LOGITAB.TAB");
    resources->frames_tab  = load_file("GAME_DIR/AR1/FIL/FRAMES.TAB");
    resources->animjoy_tab = load_file("GAME_DIR/AR1/FIL/ANIMJOY.TAB");
    resources->animofs_tab = load_file("GAME_DIR/AR1/FIL/ANIMOFS.TAB");
    resources->sostani_tab = load_file("GAME_DIR/AR1/FIL/SOSTANI.TAB");

    resources->dsp         = load_file("GAME_DIR/AR1/FIL/DSP");
    resources->usc         = load_file("GAME_DIR/AR1/FIL/USC");
    resources->prt         = load_file("GAME_DIR/AR1/FIL/PRT");
    resources->swi         = load_file("GAME_DIR/AR1/FIL/SWI");
    resources->pn          = load_file("GAME_DIR/AR1/FIL/PN");
    resources->pu          = load_file("GAME_DIR/AR1/FIL/PU");
    resources->ptx         = load_file("GAME_DIR/AR1/FIL/PTX");

    resources->texts_it    = load_file("GAME_DIR/PTX/Texts.kit");
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

void swi_elements_init(tr_game *game, tr_resources *resources) {
    uint8_t *swi = resources->swi;

    for (int i = 0; i < 0x40; i++) {
        uint16_t type  = from_big_endian(read16_unaligned(swi + 0)); (type = type);
        uint16_t count = from_big_endian(read16_unaligned(swi + 2));

        game->swi_elements[i] = swi;
        swi += count;
    }
}

#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)

int tileid_get_file(tr_tileid tileid) {
    return  ((tileid.value & 0xf000) >> 12);
}

int tileid_get_i(tr_tileid tileid) {
    return tileid.value & 0x01ff;
}

bool tileid_get_flip(tr_tileid tileid) {
    return !!(tileid.value & 0x0200);
}

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

tr_tileid room_get_tile_id(uint8_t *room_file, int room, int pos) {
    int offset = (room * 0x04f4) + 0x0024 + (pos * 2);
    return (tr_tileid){ from_big_endian(*(uint16_t *)(room_file + offset)) };
}

tr_tileid room_get_tile_id_xy(uint8_t *room_file, int room, int x, int y) {
    int pos = (screen_to_tile_y(y) * GAME_TILES_WIDTH) + screen_to_tile_x(x);
    return room_get_tile_id(room_file, room, pos);
}

void room_set_tile_id_tile_x_tile_y(uint8_t *room_file, int room, int x, int y, uint16_t type) {
    int pos = (y * GAME_TILES_WIDTH) + x;
    int offset = (room * 0x04f4) + 0x0024 + (pos * 2);
    uint16_t *ptr = (uint16_t *)(room_file + offset);
    *ptr = from_big_endian(type);
}

tr_tiletype room_get_tile_type(uint8_t *room_file, int room, int pos) {
    int offset = (room * 0x04f4) + 0x0364 + pos;
    uint16_t value = (pos < GAME_TILES_WIDTH * GAME_TILES_HEIGHT)
        ? *(room_file + offset)
        : 0xfc;
    return (tr_tiletype) { value };
}

tr_tiletype room_get_tile_type_xy(uint8_t *room_file, int room, int x, int y) {
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
                            tr_tileid tileid,
                            uint8_t *src,
                            uint8_t *dst)
{
    int  tile_i = tileid_get_i(tileid);
    bool flip   = tileid_get_flip(tileid);

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

tr_tileid adjust_bg_tile_animation(tr_tileid tileid, int frame) {
    if ((tileid.value & 0xf000) != 0x9000)
        return tileid;

    if (tileid.value & 0x0800) {
        frame = 3;
    }
    else {
        if ((tileid.value & 0x0400) && (frame == 3)) {
            tileid.value = tileid.value | 0x0800;
        }
    }

    return (tr_tileid){ (tileid.value & 0xfffc) | (frame & 0x0003) };
}

void render_background_layer(uint8_t *room_file, int room, tr_tilesets *sets, uint8_t *dst, int frame) {
    for (int i = 0; i < GAME_TILES_WIDTH * GAME_TILES_HEIGHT; i++) {
        int tile_x = i % 0x14;
        int tile_y = i / 0x14;

        tr_tileid tile_id = room_get_tile_id(room_file, room, i);
        tile_id = adjust_bg_tile_animation(tile_id, frame);

        int file_id = tileid_get_file(tile_id);
        int set_id = room_get_mat_file(room_file, room, file_id);

        if (set_id > 0xc) continue;

        uint8_t *src = sets->sets[set_id];
        if (!src) continue;

        render_background_tile(tile_x, tile_y, tile_id, src, dst);
    }
}

void add_bob_per_background(
    tr_renderer *renderer,
    int x,
    int y,
    tr_ele_kind ele,
    int ele_idx,
    int palette_start,
    int flip,
    int color
) {

    tr_render_command *cmd = tr_renderer_add_command(renderer);
    cmd->type = tr_render_command_graphics;

    cmd->command.render_graphics.x = x;
    cmd->command.render_graphics.y = y;
    cmd->command.render_graphics.palette_start = palette_start;
    cmd->command.render_graphics.flip = flip;
    cmd->command.render_graphics.color = color;
    cmd->command.render_graphics.center_bottom_anchor = true;

    cmd->command.render_graphics.res.res.ele.ele_kind = ele;
    cmd->command.render_graphics.res.res.ele.ele_idx = ele_idx;
}

void tr_render_ele_ani(uint8_t *ele_data, tr_image_8bpp *dst_item) {
    static bool console_log = false;

    uint8_t *src_item = ele_data;

    dst_item->width  = read16_unaligned(src_item + 0 * 2);
    dst_item->height = read16_unaligned(src_item + 1 * 2);

    int size = dst_item->width * dst_item->height;
    dst_item->pixels = malloc(size);
    dst_item->mask   = malloc(size);

    memset(dst_item->pixels, 0, size);
    memset(dst_item->mask,   0, size);

    uint8_t *src = src_item + 5;
    uint8_t *dst = dst_item->pixels;
    uint8_t *msk = dst_item->mask;

    int x_left = dst_item->width;

    int line = 0;
    while (line < dst_item->height) {
        uint8_t skip = *src++;

        if (skip == 0xff) {
            dst += x_left; msk += x_left;
            line++;
            if (console_log) printf("\n");
            x_left = dst_item->width;
            continue;
        }

        dst += skip;
        msk += skip;

        x_left -= skip;

        uint8_t count = *src++;
        if (count == 0xff) {
            dst += x_left; msk += x_left;
            line++;
            if (console_log) printf("\n");
            x_left = dst_item->width;
            continue;
        }

        if (console_log)
            for (int i = 0; i < skip; i++)
                printf("   ");

        for (int i = 0; i < count; i++) {
            uint8_t color = *src++;

            *dst++ = color; *msk++ = 1;

            if (console_log) printf("%02x ", color);

            x_left -= 1;
        }
    }
}

void tr_render_ele(uint8_t *ele_data, tr_image_8bpp *dst_item) {
    uint8_t *src_item = ele_data;
    uint8_t *src = src_item + 5;

    dst_item->width  = read16_unaligned(src_item + 0 * 2);
    dst_item->height = read16_unaligned(src_item + 1 * 2);

    int size = dst_item->width * dst_item->height;
    dst_item->pixels = malloc(size);
    dst_item->mask   = malloc(size);

    memset(dst_item->pixels, 0, size);
    memset(dst_item->mask,   0, size);

    for (int y = 0; y < dst_item->height; y++) {
        uint8_t x = 0;

        while (1) {
            uint8_t skip = *src++;
            if (skip == 0xff)
                break;

            x += skip;

            uint8_t count = *src++;
            if (count == 0xff)
                break;

            for (int i = 0; i < count / 2; i++) {
                uint8_t colors = *src++;
                uint8_t color1 = ((colors & 0x0f)     );
                uint8_t color2 = ((colors & 0xf0) >> 4);

                dst_item->pixels [y * dst_item->width + x  ] = color1;
                dst_item->mask   [y * dst_item->width + x++] = 1;

                dst_item->pixels [y * dst_item->width + x  ] = color2;
                dst_item->mask   [y * dst_item->width + x++] = 1;
            }

            if (count & 1) {
                uint8_t color = *src++;
                uint8_t color1 = (color & 0x0f);

                dst_item->pixels [y * dst_item->width + x  ] = color1;
                dst_item->mask   [y * dst_item->width + x++] = 1;
            }
        }
    }
}

void tr_graphics_init(tr_graphics *graphics, uint8_t *ele_file) {
    graphics->count = (*(uint16_t *)ele_file);
    graphics->items = calloc(graphics->count, sizeof(tr_image_8bpp));

    for (int i = 0; i < graphics->count; i++) {
        int           offset    = (i * 4) + 2;
        uint8_t       *src_offset = ele_file + offset;
        uint8_t       *src_item = ele_file + 2 + read32_unaligned(src_offset);

        tr_render_ele(src_item, &graphics->items[i]);
    }
}

tr_pu_item *pu_item_get(tr_resources *resources, int room, int enemy) {
    assert(sizeof(tr_pu_item) == 0x1c);
    int offset = ((room * 2) + enemy) * sizeof(tr_pu_item);
    return (tr_pu_item *)(resources->pu + offset);
}

void DrawTextureScaled(Texture texture, int x, int y, int width, int height, bool flip) {
    Rectangle sourceRect = {
        .x =  0, //!flip ? 0 : width,
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
    texts->textures = calloc(texts->count, sizeof(Texture));
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
            .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
            .mipmaps = 1,
        });

        SetTextureFilter(texts->textures[image], TEXTURE_FILTER_POINT);
    }
}

static void DrawTileTypes(const tr_resources *resources, int the_room) {
    for (int t_y = 0; t_y < GAME_TILES_HEIGHT; t_y++) {
        for (int t_x = 0; t_x < GAME_TILES_WIDTH; t_x++) {
            int x = GAME_SIZE_SCALE * t_x * TILE_WIDTH;
            int y = GAME_SIZE_SCALE * t_y * TILE_HEIGHT;

            tr_tiletype type = room_get_tile_type(resources->room_roe,
                                                  the_room,
                                                  t_y * GAME_TILES_WIDTH + t_x);

            char str[0x100];
            snprintf(str, sizeof(str), "%2x", type.value);

            if (type.value != 0)
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


void offset_from_ani(tr_game *game, tr_resources *resources) {
    uint16_t *offs = (uint16_t *)resources->animofs_tab;
    uint16_t  o1   = from_big_endian(offs[0]) + game->ani;
    uint16_t  o2   = from_big_endian(offs[1]) + game->ani;
    uint16_t  o3   = from_big_endian(offs[2]) + game->ani;
    uint16_t  o4   = from_big_endian(offs[3]) + game->ani;

    game->new_x = (
       game->x +
       *(int8_t *)(resources->animofs_tab + o1) +
       *(int8_t *)(resources->animofs_tab + o3)
    );

    game->new_y = (
       game->y +
       *(int8_t *)(resources->animofs_tab + o2) +
       *(int8_t *)(resources->animofs_tab + o4)
    );
}

void tiletype_action_8(tr_game *game, tr_resources *resources, void *swi_ptr, uint16_t *idx, uint16_t *top) {
    uint8_t *swi_element = (uint8_t *)swi_ptr;

    uint16_t count = read16_unaligned(swi_element + 0);
    count = from_big_endian(count);

    swi_element = swi_element + 2;

    for (int i = 0; i < count; i++) {
        uint16_t xy     = from_big_endian(read16_unaligned(swi_element + 0));
        uint16_t room   = from_big_endian(read16_unaligned(swi_element + 2));
        uint16_t tileid = from_big_endian(read16_unaligned(swi_element + 4));

        if (room == game->current_room) {
            unsigned int x = xy % 20;
            unsigned int y = xy / 20;

            room_set_tile_id_tile_x_tile_y(resources->room_roe,
                                           game->current_room,
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

        if (room == game->current_room) {
            unsigned int x = xy % 20;
            unsigned int y = xy / 20;

            room_set_tile_type_tile_x_tile_y(resources->room_roe,
                                             game->current_room,
                                             x,
                                             y,
                                             tiletype);
        }

        swi_element = swi_element + 4;
    }
}

void do_tiletype_actions_inner(tr_game *game, tr_resources *resources, uint16_t boh) {
    boh = boh & 0xff;

    uint16_t idx = boh == 0xff ? 0    : boh;
    uint16_t top = boh == 0xff ? 0x3f : boh;

    for (uint16_t i = idx; i <= top; i++) {
        if (!game->swivar2[i])
            continue;

        void     *ptr1 = game->swi_elements[i];
        uint8_t  *ptr2 = (((uint8_t *)ptr1) + 4);

        uint16_t  type = read16_unaligned(ptr1);
        type = from_big_endian(type);

        if (type != 0) {
//            *background_ani_frame = 0;

            if (type == 8) {
                tiletype_action_8(game, resources, ptr2, &i, &top);
            }
            else {
                printf("%02x No tiletype action yet", type);
            }
        }
    }
}

void do_tiletype_actions(tr_game *game, tr_resources *resources, char boh) {
    if (boh > 0x3f)
        return;

    if (game->swivar2[boh] == false) {
        game->swivar2[boh] = true;

        do_tiletype_actions_inner(game, resources, boh);
    }
}

void check_and_load_ucci(tr_game *game, tr_resources *resources) {
    uint8_t wanted_ucci[2] = { 0xff, 0xff };

    for (int ucci_slot = 0; ucci_slot < 2; ucci_slot++) {
        tr_pu_item *pu_item = pu_item_get(resources,
                                          game->current_room,
                                          ucci_slot);

        if ((pu_item->boh1 > 0) ||
            (pu_item->bool1) ||
            (pu_item->x1 <= 0x140))
        {
            game->ucci_in_use[ucci_slot] = 1;

            if (pu_item->maybe_item_type == 0) {
                game->byte_2197a[ucci_slot] = 0;

                if (ucci_slot == 0) {
                    pu_item->maybe_item_type = 1;
                }
                else {
                    pu_item->maybe_item_type = 0;
                }

                wanted_ucci[ucci_slot] = pu_item->wanted_ucci_nr;
            }
        }
        else {
            wanted_ucci[ucci_slot] = 0xff;
        }
    }

    if (wanted_ucci[0] == 0xff) {
        if (wanted_ucci[1] != 0xff) {
            wanted_ucci[0] = wanted_ucci[1];
        }
    }

    game->wanted_ucci_nr = wanted_ucci[0];

    if (game->current_ucci_nr != game->wanted_ucci_nr) {
        if (game->wanted_ucci_nr != 0xff) {
            game->current_ucci_nr = game->wanted_ucci_nr;
            game->current_ucci = tr_ele_ucci0 + game->current_ucci_nr;

            // load_ucci_file(*wanted_ucci_nr);
        }
    }

    if (game->wanted_ucci_nr != 0xff) {
//        install_ucci_palette();
    }
}


void change_room(tr_game *game, tr_resources *resources, int room_to_change) {
    char previous_room = game->current_room;

    // reset_clicked_button();

    game->new_x = game->x;
    game->new_y = game->y;

//    previous_room = *no_previous_room ? 0 : *current_room_number;
//    *wanted_room = room_to_change;

//    if (!*need_to_save_arc_lzr && !*maybe_exit_related) {
//        gsa_and_exit_room(*wanted_room);
//    }

    game->new_x = game->x;
    game->new_y = game->y;


    game->current_room = room_to_change;
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

        if (game->read_from_usc) {
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
                    uint16_t y_from = from_big_endian(read16_unaligned(current_usc + 4));
                    uint16_t y_to   = from_big_endian(read16_unaligned(current_usc + 2));

                    if (y_from == game->new_y) {
                        game->new_y = y_to;
                        game->y     = y_to;
                        break;
                    }
                }

                current_usc += 6;
            }
        }
        else {
            uint8_t *current_prt = resources->prt;

            while (1) {
                uint8_t room_from = *(current_prt + 0);
                uint8_t room_to   = *(current_prt + 1);

                if (room_from == 0xff && room_to == 0xff) {
                    break;
                }

                if ((room_from == previous_room) &&
                    (room_to == room_to_change))
                {
                    uint16_t x_to   = from_big_endian(read16_unaligned(current_prt + 2));
                    uint16_t y_to   = from_big_endian(read16_unaligned(current_prt + 4));
                    uint16_t x_from = from_big_endian(read16_unaligned(current_prt + 6));
                    uint16_t y_from = from_big_endian(read16_unaligned(current_prt + 8));

                    if ((x_from == (game->new_x - 0x10)) &&
                        (y_from == (game->new_y - 0x0a)))
                    {
                        game->x = game->new_x = x_to + 0x10;
                        game->y = game->new_y = y_to + 0x0a;
                    }
                }

                current_prt += 10;
            }
        }

        game->to_set_x = game->new_x;
        game->to_set_y = game->new_y;

        do_tiletype_actions_inner(game, resources, 0xffff);

        // calls_funcptr_1();

        game->tile_top    = room_get_tile_type_xy(resources->room_roe, game->current_room,
                                                  game->new_x, game->new_y     );

        game->tile_bottom = room_get_tile_type_xy(resources->room_roe, game->current_room,
                                                  game->new_x, game->new_y + 10);

         if (game->read_from_usc) {
             if (logi_tab_contains(resources->logi_tab, game->tile_top.value, 0x27)) {

#if 0
                 int16_t punto_x = game->new_x;
                 int16_t punto_y = game->new_y;
                 tr_tiletype tiletype;

                 do {
                     tiletype = room_get_tile_type_xy(resources->room_roe,
                                                      game->current_room,
                                                      punto_x,
                                                      punto_y);
                     punto_x -= 0x10;
                 } while (punto_x >= 0 && tiletype.value == 0xfd);

                 punto_x += 0x20;
                 punto_y -= 0x0a;

                 tr_tileid tileid = room_get_tile_id_xy(resources->room_roe,
                                                        game->current_room,
                                                        punto_x,
                                                        punto_y);
#endif
             }
         }

        game->read_from_usc = false;
    }

    // *no_previous_room = 0;

    // reset_array_bobs();
    check_and_load_ucci(game, resources);
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


void change_at_screen(tr_game *game, tr_resources *resources) {
    static uint8_t stru_121f0[] = {
        0x0E, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x1C,
        0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    char direction = game->ani >= 0x35;

    int pupo_real_x = game->x + game->x_delta;

    if ((pupo_real_x > 319 && direction) ||
        (pupo_real_x < 0 && !direction))
    {
        if (!set_is_member(game->ani, stru_121f0)) {
            game->read_from_usc = true;

            game->new_x = game->x > 0 ? game->x - 320 : game->x + 320;
            game->x = pupo_real_x > 0 ? 312 : 8;


            {
                int y = game->y - 0xa;
                char new_room;
                char tile_type;

                do {

                    new_room = room_get_tile_type_xy(resources->room_roe, game->current_room, game->x, y).value;

                    y += 0xa;

                    tile_type = room_get_tile_type_xy(resources->room_roe, game->current_room, game->x, y).value;

                    y += 0xa;

                    if (logi_tab_contains(resources->logi_tab, tile_type, 0x25))
                        break;
                } while (y <= 0xc7);

                game->x = game->new_x;
                change_room(game, resources, new_room);
            }
        }
    }
}

void cosa_ho_di_fronte(tr_game *game, tr_resources* resources, uint8_t *ani, int x, int y, int nx, int ny) {
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
            tr_tiletype tiletype = room_get_tile_type_xy(resources->room_roe, game->current_room, x + xoff, y);

            if (logi_tab_contains(resources->logi_tab, tiletype.value, logitab_idx)) {
                new_ani = ani_from_file;
                changed = 1;
                break;
            }

            xoff += incr;
        }
    } while (changed);

    *ani = new_ani;
}

void cambia_il_salto(tr_resources *resources, uint8_t *ani, tr_tiletype top) {
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

            if (logi_tab_contains(resources->logi_tab, top.value, logitab_index))
                *ani = new_ani;

        }

        sostani_item += 3;
    }

    // printf("\n");
}

static uint8_t stru_116f1[] = {
    0x81, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static uint8_t stru_11dd9[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static uint8_t stru_11df9[] = {
    0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void controlla_sotto_piedi(
    tr_game *game,
    tr_resources *resources,
    uint8_t  *ani_ptr,
    tr_tiletype top,
    tr_tiletype bottom,
    uint16_t x,
    uint16_t y,
    uint16_t new_x,
    uint16_t new_y
) {
    bool direction = game->ani >= 0x35;

    int  var2 = 0;
    tr_tiletype tile_type;

    if (game->byte_1f4e6 == 0 && top.value == 0xfc) {
        game->byte_1f4e6 = 7;
        tile_type = room_get_tile_type_xy(resources->room_roe,
                                          game->current_room,
                                          game->x,
                                          game->y - 0x0a);


        if (tile_type.value != 0) {
            /* game->pupo_bob_palette_override = 0xc7 - game->palette_delta */
        }

        *ani_ptr = direction ? 0x67 : 0x32;
        game->counter_caduta = game->counter_caduta + 1;
        return;
    }

    if (logi_tab_contains(resources->logi_tab, bottom.value, 0x25)) {
        *ani_ptr = direction ? 0x67 : 0x32;
        game->counter_caduta = game->counter_caduta + 1;
        return;
    }

    if (game->byte_1f4e6 > 0) {
        /* loc_11ea8 */
        bool facing_right = direction;

        game->byte_1f4e6 = game->byte_1f4e6 - 1;

        if (game->byte_1f4e6 == 0)
            /* || (*has_to_adjust_ani != 0))*/
        {
            if ((game->to_set_x & 0xf) != 8) {
                game->to_set_x = 8;
                game->to_set_y = 0xb4;
                facing_right = true;
            }
            else if ((game->to_set_y % 0xa) != 0) {
                game->to_set_y = 0xbe;
            }

            game->x = game->to_set_x;
            game->y = game->to_set_y;

            int x_step = facing_right ? -0x10 : 0x10;
            int y_step = -0x0a;

            game->tile_top = room_get_tile_type_xy(resources->room_roe,
                                                   game->current_room,
                                                   game->x,
                                                   game->y);

            game->tile_bottom = room_get_tile_type_xy(resources->room_roe,
                                                      game->current_room,
                                                      game->x,
                                                      game->y + 10);

            while ((game->y <= 190) &&
                   (logi_tab_contains(resources->logi_tab, game->tile_bottom.value, 0x25) ||
                    set_is_member(game->tile_top.value, stru_116f1)))
            {
                while ((game->x >= 8) &&
                       (game->x <= 312) &&
                       (logi_tab_contains(resources->logi_tab, game->tile_bottom.value, 0x25) ||
                        set_is_member(game->tile_top.value, stru_116f1)))
                {
                    game->x += x_step;

                    game->tile_top = room_get_tile_type_xy(resources->room_roe,
                                                           game->current_room,
                                                           game->x,
                                                           game->y);

                    game->tile_bottom = room_get_tile_type_xy(resources->room_roe,
                                                              game->current_room,
                                                              game->x,
                                                              game->y + 10);
                }

                if (logi_tab_contains(resources->logi_tab, game->tile_bottom.value, 0x25) ||
                    set_is_member(game->tile_top.value, stru_116f1))
                {
                    game->x = game->to_set_x;
                }

                while ((game->x >= 8) &&
                       (game->x <= 312) &&
                       (logi_tab_contains(resources->logi_tab, game->tile_bottom.value, 0x25) ||
                        set_is_member(game->tile_top.value, stru_116f1)))
                {
                    game->x += x_step;

                    game->tile_top = room_get_tile_type_xy(resources->room_roe,
                                                           game->current_room,
                                                           game->x,
                                                           game->y);

                    game->tile_bottom = room_get_tile_type_xy(resources->room_roe,
                                                              game->current_room,
                                                              game->x,
                                                              game->y + 10);
                }

                if (logi_tab_contains(resources->logi_tab, game->tile_bottom.value, 0x25) ||
                    set_is_member(game->tile_top.value, stru_116f1))
                {
                    game->x = game->to_set_x;
                    game->y += y_step;

                    game->tile_top = room_get_tile_type_xy(resources->room_roe,
                                                           game->current_room,
                                                           game->x,
                                                           game->y);

                    game->tile_bottom = room_get_tile_type_xy(resources->room_roe,
                                                              game->current_room,
                                                              game->x,
                                                              game->y + 10);

                    if (game->y <= 0xa) {
                        game->y = game->to_set_y;
                        y_step = 0xa;
                    }
                }
            }

            game->new_x = game->to_set_x = game->x;
            game->new_y = game->to_set_y = game->y;

            /* game->bob_bg_palette_override = 0; */

            game->frame_nr = 0;
            game->ani = facing_right ? 0x38 : 0x03;
            game->countdown = 0;
            game->get_new_ani = 0;
            game->disable_ani = 1;
            game->byte_1f4e6 = 0;
            game->counter_caduta = 0;
        }
        else {
            *ani_ptr = direction ? 0x67 : 0x32;
            game->counter_caduta = game->counter_caduta + 1;
        }

        return;
    }

    if (game->byte_1f4e6 > 0) {
        /* loc_11ecf */
        printf("NOPE 3\n");
        return;
    }

    if (top.value == 9) {
        /* fai cade */
        printf("NOPE 4\n");
        return;
    }

    if (top.value == 0) {
        *ani_ptr = direction ? 0x67 : 0x32;
        game->counter_caduta = game->counter_caduta + 1;
        return;
    }

    if (game->counter_caduta > 0) {
        /* loc_11f3c */

        if (game->counter_caduta > 8) {
            *ani_ptr = direction ? 0x68 : 0x33;
        }

        game->counter_caduta = 0;
        return;
    }

    if (game->byte_1f4e8 > 0) {
        game->byte_1f4e8--;

        if (set_is_member(*ani_ptr, stru_11dd9)) {
            /* set ani 38_3 */
            *ani_ptr = direction ? 0x38 : 0x03;
            var2 = 1;
        }
    }

    if (!var2) {
        if (game->byte_1f4e9 > 0) {
            game->byte_1f4e9--;

            if (set_is_member(*ani_ptr, stru_11df9)) {
                /* set ani 38_3 */
                *ani_ptr = direction ? 0x38 : 0x03;
                var2 = 1;
            }
        }
    }

    if (!var2) {
        cambia_il_salto(resources, ani_ptr, top);
    }

    if ((*ani_ptr == 0x12) || (*ani_ptr == 0x13)) {
        /* sub_11a14 */
        if (!game->gun_bool && game->colpi == 0) {
            *ani_ptr = 0x3;
            game->gun_bool = 1;

            // sub_11984();
        }

    }

    if ((*ani_ptr == 0x47) || (*ani_ptr == 0x48)) {
        /* sub_11a3d */
        if (!game->gun_bool && game->colpi == 0) {
            *ani_ptr = 0x38;
            game->gun_bool = 1;

            // sub_11984();
        }

    }

    if (set_is_member(*ani_ptr, stru_11dd9)) {
        /* loc_11ff6 */
        game->byte_1f4e8 = 4;
        return;
    }

    if (set_is_member(*ani_ptr, stru_11df9)) {
        /* loc_12011 */
        game->byte_1f4e9 = 4;
        return;
    }

    if (*ani_ptr == 0x14) {
        /* loc_12027 */
        printf("NOPE 10\n");
        return;
    }

    if (*ani_ptr == 0x49) {
        /* loc_12036 */
        printf("NOPE 11\n");
        return;
    }

    if (*ani_ptr == 0x8) {
        /* loc_12045 */
        printf("NOPE 12\n");
        return;
    }

    if (*ani_ptr == 0x9) {
        /* loc_12054 */
        printf("NOPE 13\n");
        return;
    }

    if (*ani_ptr == 0x3d) {
        /* loc_12063 */
        printf("NOPE 14\n");
        return;
    }

    if (*ani_ptr == 0x3e) {
        /* loc_12072 */
        printf("NOPE 15\n");
        return;
    }


    if (*ani_ptr == 0x10) {
        if (logi_tab_contains(resources->logi_tab, top.value, 0x26) ||
            logi_tab_contains(resources->logi_tab, top.value, 0x28))
         {
            if (new_x - 10 >= 0) {
                tile_type = room_get_tile_type_xy(resources->room_roe,
                                                  game->current_room,
                                                  new_x - 10,
                                                  new_y);

                if (tile_type.value == top.value) {
                    *ani_ptr = 0x1d;
                }
            }
        }

        return;
    }

    if (*ani_ptr == 0x45) {
        /* loc_12088 */
        if (logi_tab_contains(resources->logi_tab, top.value, 0x26) ||
            logi_tab_contains(resources->logi_tab, top.value, 0x28))
         {
            if ((new_x + 10) < 320) {
                tile_type = room_get_tile_type_xy(resources->room_roe,
                                                  game->current_room,
                                                  new_x + 10,
                                                  new_y);

                if (tile_type.value == top.value) {
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
        if (logi_tab_contains(resources->logi_tab, top.value, 0x28)) {
            uint8_t the_ani = *ani_ptr;

            game->wanted_room = game->current_room;

            /*
            do_tiletype_actions(top_copy - 0xc0);
            rese();
            calls_funcptr_1();
            */

            if (game->wanted_room != game->current_room) {
                change_room(game, resources, game->wanted_room);
            }

            if (the_ani != *ani_ptr) {
                *ani_ptr = 0x22;
            }
        }
        else {
            if (top.value == 0xa8) {
                *ani_ptr = 0x20;
            }
            else if (top.value == 0xfe) {
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

        tr_tiletype pupo_tile = room_get_tile_type_xy(resources->room_roe,
                                                      game->current_room,
                                                      new_x,
                                                      new_y);

        if (!logi_tab_contains(resources->logi_tab, pupo_tile.value, 0x25) ||
            !logi_tab_contains(resources->logi_tab, pupo_tile.value, 0x27))
        {
            tile_type = room_get_tile_type_xy(resources->room_roe,
                                              game->current_room,
                                              new_x,
                                              new_y - 0x0a);

            change_room(game, resources, tile_type.value);
        }

        return;
    }
}

void add_vita(tr_game *game, int x) {
    game->faccia_countdown = 0x96;
    game->punti_countdown = 0;
    /* *byte_21ab8 = 0 */
    game->vita += x;
}

void add_score(tr_game *game, int x) {
    game->punti_countdown = 0x96;
    game->faccia_countdown = 0;
    /* *byte_21ab8 = 0; */

    game->score += x;
}

void do_damage(tr_game *game, tr_resources *resources) {
    if (logi_tab_contains(resources->logi_tab, game->tile_top.value, 0x34)) {
        add_vita(game, 0x24);
        /* maybe_hurt_fx(0xffff, 0x0f, 0x0f); */
        game->palette_mangling = 5;
    }
}

uint8_t stru_17197[] = {
    0x00, 0x00, 0x00, 0x90, 0x01, 0x00, 0x10, 0x00,
    0x00, 0x00, 0x02, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t stru_171B7[] = {
    0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

bool check_pu_for_vita(tr_game *game, tr_resources *resources, int enemy) {
    tr_pu_item *pu_item = pu_item_get(resources, game->current_room, enemy);

    if (pu_item->ignored1 == 5) {
        if (game->y == pu_item->y1) {
            int dx = enemy * 0x1c;
            int ax = pu_item->x1 - game->x;
            ax = (ax ^ dx) - dx;

            if ((ax <= 0x20) &&
                !set_is_member(game->ani, stru_17197) &&
                set_is_member(pu_item->maybe_item_type, stru_171B7))
            {
                add_vita(game, pu_item->vita * 8 / 5);
                return true;
            }
        }
    }

    return false;
}

void update_pupo(tr_game *game, tr_resources *resources, uint8_t direction, int force_ani) {
    char enemy_hit = 0;
    char get_new_frame = 0;

    do {
        if (!enemy_hit)
            get_new_frame = 0;

        if (game->get_new_ani && !enemy_hit)
        {
            game->ani = resources->animjoy_tab[game->ani * 18 + direction];
            if (force_ani > 0x10000) {
                game->ani = force_ani - 0x10000;
                game->x = 150;
                game->y = 0xa0;
            }

            game->get_new_ani = 0;
            get_new_frame = 1;

            game->tile_top = room_get_tile_type_xy(resources->room_roe,
                                                   game->current_room,
                                                   game->x,
                                                   game->y);

            game->tile_bottom = room_get_tile_type_xy(resources->room_roe,
                                                   game->current_room,
                                                   game->x,
                                                   game->y + 10);

            controlla_sotto_piedi(game,
                                  resources,
                                  &game->ani,
                                  game->tile_top,
                                  game->tile_bottom,
                                  game->x,
                                  game->y,
                                  game->new_x,
                                  game->new_y);

            /*
            if (*gun_bool == 0) {
                int a = *pupo_current_ani;

                *pupo_current_ani =
                    (a == 0x47) ? 0x48:
                    (a == 0x12) ? 0x13:
                    a;
            }
             */

            int a = game->ani;

            do {
                offset_from_ani(game, resources);

                cosa_ho_di_fronte(game,
                                  resources,
                                  &(game->ani),
                                  game->x,
                                  game->y,
                                  game->new_x,
                                  game->new_y);
                a = game->ani;
            } while (game->ani != a);
        }

        if (game->disable_ani) {
            get_new_frame = 1;
            game->disable_ani = 0;
        }

        // gsa_and_exit();

        if (game->countdown == 0) {
            if (get_new_frame) {
                game->byte_1f4dc = 0;

                uint8_t *off = resources->frames_tab + (game->ani * 2);
                game->pupo_offset = from_big_endian(*(uint16_t *)(off));

                uint16_t *offs = (uint16_t *)resources->animofs_tab;
                uint16_t  o1   = from_big_endian(offs[0]) + game->ani;
                uint16_t  o2   = from_big_endian(offs[1]) + game->ani;

                game->x += *(int8_t *)(resources->animofs_tab + o1);
                game->y += *(int8_t *)(resources->animofs_tab + o2);
            }

            uint8_t *frame_info = resources->frames_tab + game->pupo_offset;

            uint8_t frame = frame_info[0];
            uint8_t time  = frame_info[1];

            if ((frame + time) != 0) {
                uint8_t x_delta = frame_info[2];
                uint8_t y_delta = frame_info[3];
                uint8_t flip    = frame_info[4];

                game->pupo_offset += 5; /* sizeof(frame_info_t) */
                game->frame_nr  = frame;
                game->countdown = (time + 3) / 4;
                game->x_delta   = x_delta;
                game->y_delta   = y_delta;
                game->flip      = flip;
            }
            else {
                game->get_new_ani = true;
            }

            if (game->get_new_ani) {
                uint16_t *offs = (uint16_t *)resources->animofs_tab;
                uint16_t  o3   = from_big_endian(offs[2]) + game->ani;
                uint16_t  o4   = from_big_endian(offs[3]) + game->ani;

                game->x += *(int8_t *)(resources->animofs_tab + o3);
                game->y += *(int8_t *)(resources->animofs_tab + o4);

                if ((game->to_set_x < 0) ||
                    (game->to_set_x > 319))
                {
                    game->to_set_x = game->x;
                    game->to_set_y = game->y;
                }
            }

            game->byte_1f4dc++;

            if ((game->tile_top.value & 0xf0) == 0xd0)
            {
                char type = (game->tile_top.value & 0x0f) + 0xc;

                do_tiletype_actions(game, resources, type);
                // reset_clicked_button();
                // calls_funcptr_1();

                {
                    game->new_x = game->x;
                    game->new_y = game->y;

                    game->tile_top    = room_get_tile_type_xy(resources->room_roe,
                                                              game->current_room,
                                                              game->x,
                                                              game->y);

                    game->tile_bottom = room_get_tile_type_xy(resources->room_roe,
                                                              game->current_room,
                                                              game->x,
                                                              game->y + 10);

                    // reset_array_bobs();
                    // reset_background_ani();

                    if (game->vita > 0x800) {
                        // animate_and_render_background();
                        // draw_stars();
                        // draw_punti_faccia();
                    }

                    {
                        // copy_bg_to_vga();

                        // select_pal(1, 0);
                        // install_ucci();

                        game->palette_mangling = 2;
                    }
                }
            }

            do_damage(game, resources);

            enemy_hit = 1;

            if (!check_pu_for_vita(game, resources, 0) &&
                !check_pu_for_vita(game, resources, 1))
            {
                enemy_hit = 0;
            }

            if (enemy_hit) {
                game->get_new_ani = 1;
                get_new_frame = 1;
                game->countdown = 0;
                game->ani = game->ani > 0x35 ? 0x51: 0x1c;
            }
            else {
                if (game->byte_1f4dc == 4) {
                    // if (set_is_member(*pupo_current_ani, MK_FP(seg002, 0x176d))) {
                    //     mangle_pu_gun();
                    //     reset_clicked_button();
                    // }
                }
            }
        }
    } while (game->get_new_ani);

    change_at_screen(game, resources);

    // sub_122f1();

    if (!game->disable_ani) {
        game->countdown--;
    }
}

void add_pupo_to_bobs(tr_game *game, tr_renderer *renderer) {
    add_bob_per_background(renderer,
                           game->x + game->x_delta,
                           game->y + game->y_delta,
                           tr_ele_tr,
                           game->frame_nr,
                           0xffc1,
                           game->flip,
                           0 // game->palette_override
                           );

    // *bob_to_hit_mouse_3 = *bobs_count;
}

void add_enemy_to_bobs(tr_game *game, tr_resources *resources, tr_renderer *renderer, int enemy_id) {
    tr_pu_item *pu_item = pu_item_get(resources,
                                      game->current_room,
                                      enemy_id);


    int x = from_big_endian(pu_item->x1) + from_big_endian(pu_item->x2);
    int y = from_big_endian(pu_item->y1) + from_big_endian(pu_item->y2);

    if (!pu_item->bool1 || x < 320) {
        add_bob_per_background(renderer,
                               x,
                               y,
                               game->current_ucci,
                               pu_item->sprite_nr,
                               0xffd1,
                               game->enemy_flip[enemy_id],
                               0);
    }

}

void draw_pupi(tr_game *game, tr_resources *resources, tr_renderer *renderer) {
    /* sort not implemented */

    add_enemy_to_bobs(game, resources, renderer, 0);
    add_enemy_to_bobs(game, resources, renderer, 1);
    add_pupo_to_bobs(game, renderer);
}

void draw_stars_and_check(tr_game *game, tr_resources *resources, tr_renderer *renderer) {
    game->stars_countdown--;

    if (game->stars_countdown == 0) {
        game->stars_sprite_nr++;
        game->stars_countdown = 4;
    }

    if (game->stars_sprite_nr >= 0x10) {
        game->stars_sprite_nr = 0;
    }

    uint8_t sprite_nr = game->stars_sprite_nr & 3;

    uint8_t *pn = resources->pn;

    while (1) {
        unsigned int room = from_big_endian(read16_unaligned(pn + 0));

        if (room == 0xffff) {
            break;
        }

        uint16_t score = from_big_endian(read16_unaligned(pn + 2));
        uint16_t x     = from_big_endian(read16_unaligned(pn + 4));
        uint16_t y     = from_big_endian(read16_unaligned(pn + 6));

        if (room == game->current_room) {
            add_bob_per_background(renderer,
                                   x,
                                   y - 0x1e,
                                   tr_ele_k,
                                   sprite_nr,
                                   0xfff0,
                                   0,
                                   0);

            if ((x == game->new_x) &&
                (y == game->new_y))
            {
                add_score(game, score);
                /* maybe_hurt_fx(0xff96, 0xff96, 0xff96); */
                game->palette_mangling = 5;
                *(uint16_t *)(pn) = from_big_endian(0xfffe);
            }
        }

        pn = pn + 8;
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
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1,
    });

    SetTextureFilter(bg->texture, TEXTURE_FILTER_POINT);
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

void draw_faccia(tr_game *game, tr_renderer *renderer) {
    if (game->faccia_countdown == 0) {
        return;
    }

    game->faccia_countdown--;

    int y = game->y < 0x64 ? 0xc3 : 0x34;
    int boh = y - 7 - ((0x800 - game->vita) / 0x31); boh = boh;

    add_bob_per_background(renderer, 0x123, y,      tr_ele_status, 1, 0xfff0, 0, 0);
    add_bob_per_background(renderer, 0x123, y + 50, tr_ele_status, 0, 0xfff0, 0, 0);
}

void draw_punti(tr_game *game, tr_renderer *renderer) {
    if (game->punti_countdown == 0) {
        return;
    }

    game->punti_countdown--;

    char punti[0x20];
    int len = snprintf(punti, sizeof(punti), "%d", game->score);

    for (int i = len - 1; i >= 0; i--) {
        add_bob_per_background(renderer,
                               319 - (len << 4) - 8 + (i << 4),
                               game->y > 0x64 ? 0x1a : 0x3c,
                               tr_ele_numeri,
                               punti[i] - '0',
                               0xfff0,
                               0,
                               0);
    }
}

void tr_arcade_gameloop_tick(tr_renderer *renderer,
                             tr_game *game,
                             tr_resources *resources,
                             uint8_t direction) {
    tr_renderer_clear(renderer);

    tr_render_command *bg = tr_renderer_add_command(renderer);
    bg->type = tr_render_command_arcade_background;

    draw_faccia(game, renderer);
    draw_punti(game, renderer);
    draw_stars_and_check(game, resources, renderer);
    update_pupo(game, resources, direction, 0);
    // check gsa & exit
    // load & update ucci 0
    // load & update ucci 1
    // clear bob mouse
    draw_pupi(game, resources, renderer);
    // check mouse
    // draw mouse
    // reset bobs
}

void get_explanation(tr_game *game, tr_resources *resources, uint16_t *out_line1, uint16_t *out_line2) {
    uint8_t  *ptx = resources->ptx;
    uint16_t count = from_big_endian(read16_unaligned(ptx + 0x10));

    ptx += 0x12;

    *out_line1 = 0xffff;
    *out_line2 = 0xffff;

    for (int i = 0; i < count; i++) {
        uint16_t room  = from_big_endian(read16_unaligned(ptx)); ptx += 2;
        uint16_t min_x = from_big_endian(read16_unaligned(ptx)); ptx += 2;
        uint16_t min_y = from_big_endian(read16_unaligned(ptx)); ptx += 2;
        uint16_t max_x = from_big_endian(read16_unaligned(ptx)); ptx += 2;
        uint16_t max_y = from_big_endian(read16_unaligned(ptx)); ptx += 2;
        uint16_t items = from_big_endian(read16_unaligned(ptx)); ptx += 2;

        if ((game->current_room == room) &&
            (game->x >= min_x) &&
            (game->x <= max_x) &&
            (game->y >= min_y) &&
            (game->y <= max_y))
        {
            for (int k = 0; k < items; k++) {
                uint8_t *item  = ptx;
                uint16_t swi   = from_big_endian(read16_unaligned(item)); item += 2;
                uint16_t line1 = from_big_endian(read16_unaligned(item)); item += 2;
                uint16_t line2 = from_big_endian(read16_unaligned(item)); item += 2;

                if (swi == 0xffff) {
                    *out_line1 = line1;
                    *out_line2 = line2;
                }
                else if (game->swivar2[swi]) {
                    *out_line1 = line1;
                    *out_line2 = line2;
                }
            }
        }

        ptx += 5 /* items */ * 6 /* size */;
    }
}

void get_pti_line(tr_resources *resources, uint16_t line_idx, char *line_str) {
    uint8_t *pti = resources->texts_it + 0x40;

    uint8_t xor = *(resources->texts_it + 0x102);

    for (int i = 0; i < 0x10; i++) {
        uint8_t  *item  = pti + i * 0xc;
        uint16_t first  = *(uint16_t *)(item + 0); first = first;
        uint16_t last   = *(uint16_t *)(item + 2); last = last;
        uint16_t count  = *(uint16_t *)(item + 4);
        uint16_t size   = *(uint16_t *)(item + 6); size = size;
        uint32_t offset = *(uint32_t *)(item + 8);

        uint8_t  *item_ptr = resources->texts_it + offset;

        for (int k = 0; k < count; k++) {
            uint16_t line   = read16_unaligned(item_ptr); item_ptr += 2;
            uint8_t  length = *(item_ptr);                item_ptr += 1;

            for (int s = 0; s < length; s++) {
                if (line == line_idx)
                    line_str[s] = *(item_ptr) ^ xor;
                item_ptr++;
            }

            if (line == line_idx) {
                line_str[length] = 0;
                return;
            }

            item_ptr++;
        }
    }
}

void tr_game_reset(tr_game *game, tr_resources *resources) {
    memset(game, 0, sizeof(tr_game));

    game->current_room = 0x00;
    game->x = 0xb8;
    game->y = 0xa0;
    game->ani = 0x03;

    game->frame_nr = 0x00;
    game->flip = 0;
    game->countdown = 0;
    game->get_new_ani = 1;
    game->stars_countdown = 4;
    game->gun_bool = 1;
    game->current_ucci = tr_ele_ucci0;

    swi_elements_init(game, resources);
}

typedef struct {
    tr_resources resources;
    tr_palette   palette;
    tr_tilesets  tilesets;

    tr_graphics status_ele;
    tr_graphics numeri_ele;
    tr_graphics k_ele;
    tr_graphics tr_ele;
    tr_graphics ucci0_ele;
    tr_graphics ucci1_ele;

    tr_bg bg;
    tr_game game;
} tr_gameloop;

void tr_gameloop_init(tr_gameloop *loop) {
    resources_load(&loop->resources);

    palette_init(&loop->palette, &loop->resources);
    tilesets_init(&loop->tilesets, &loop->resources);

    tr_graphics_init(&loop->status_ele, loop->resources.status_ele);
    tr_graphics_init(&loop->numeri_ele, loop->resources.numeri_ele);
    tr_graphics_init(&loop->k_ele, loop->resources.k_ele);
    tr_graphics_init(&loop->tr_ele, loop->resources.tr_ele);
    tr_graphics_init(&loop->ucci0_ele, loop->resources.ucci0_ele);
    tr_graphics_init(&loop->ucci1_ele, loop->resources.ucci1_ele);

    tr_game_reset(&loop->game, &loop->resources);
}

const int tr_wdw_image_count = 9;

typedef struct {
    tr_graphics images;
    tr_palette palette;
} tr_wdw;

void tr_wdw_init_palette(tr_palette *palette, uint8_t *data) {
    // uint8_t start = data[0]; // (?)
    // uint8_t end   = data[1]; // (?)
    uint8_t *color_start = data + 5;

    for (int i = 0; i < 0x100; i++) {
        palette->color[i] = 0xff000000 |
            (color_start[(i * 3) + 0] <<  2) |
            (color_start[(i * 3) + 1] << 10) |
            (color_start[(i * 3) + 2] << 18);
    }
}

void tr_wdw_init(tr_wdw *wdw, uint8_t *wdw_file) {
    wdw->images.count = tr_wdw_image_count;
    wdw->images.items = calloc(tr_wdw_image_count, sizeof(tr_image_8bpp));

    for (int i = 0; i < tr_wdw_image_count; i++) {
        uint16_t ele_offset = *((uint16_t *)(wdw_file + i * 2));
        tr_render_ele(wdw_file + ele_offset, &wdw->images.items[i]);
    }

    uint16_t palette_offset = *((uint16_t *)(wdw_file + 18));
    uint8_t *palette_data = wdw_file + palette_offset;
    tr_wdw_init_palette(&wdw->palette, palette_data);
}

static const char *wdw_files[] = {
    "TR.TIL",
    "GHOST.TIL",
    "VUOTI.TIL",
};

const int wdw_files_count = sizeof(wdw_files) / sizeof(char *);

typedef struct {
    int current;
    int scale;

    bool inited[wdw_files_count];
    tr_wdw wdws[wdw_files_count];
    ray_textures textures[wdw_files_count];
} dbg_wdw;

void dbg_wdw_init(dbg_wdw *test) {
    memset(test, 0, sizeof(dbg_wdw));
    test->scale = 1;
}

typedef struct {
    tr_palette palette;
    tr_graphics images;
} tr_ani_file;

void tr_ani_file_init(tr_ani_file *ani, uint8_t *ani_file) {
    ani->images.count = read16_unaligned(ani_file + 2) - 1;
    uint32_t palette_offset = read32_unaligned(ani_file + 10);

    uint8_t *items = ani_file + 14;
    uint8_t *palette_data = ani_file + palette_offset + 15;

    for (int i = 0; i < 0x100; i++) {
        ani->palette.color[i] = 0xff000000 |
            (palette_data[(i * 3) + 0] <<  2) |
            (palette_data[(i * 3) + 1] << 10) |
            (palette_data[(i * 3) + 2] << 18);
    }

    ani->images.items = calloc(ani->images.count, sizeof(tr_image_8bpp));

    for (int i = 0; i < ani->images.count; i++) {
        uint32_t offset = read32_unaligned(items + i * 4);
        uint8_t *ele_data = ani_file + 10 + offset;
        tr_render_ele_ani(ele_data, &ani->images.items[i]);
    }
}

static const ImVec2 zero  = { 0 };
static const ImVec2 one   = { 1, 1 };
static const ImVec4 zero4 = { 0 };
static const ImVec4 white = { 1, 1, 1, 1 };

static const char* ani_files[] = {
    "ANID01.ANI",
    "ANID02.ANI",
    "ANID03.ANI",
    "ANID04.ANI",
    "ANID05.ANI",
    "ANID06.ANI",
    "ANIG01.ANI",
    "ANIG02.ANI",
    "ANIG03.ANI",
    "ANIG03B.ANI",
    "ANIG04.ANI",
    "ANII01.ANI",
    "ANIM01.ANI",
    "ANIM03.ANI",
    "ANIM04.ANI",
    "ANIM05.ANI",
    "ANIM06.ANI",
    "ANIM54.ANI",
    "ANIO01.ANI",
    "ANIP01.ANI",
    "ANIS00.ANI",
    "ANIS01.ANI",
    "ANIS01A.ANI",
    "ANIS01B.ANI",
    "ANIS02.ANI",
    "ANIS03.ANI",
    "ANIS03A.ANI",
    "ANIS03B.ANI",
    "ANIS03C.ANI",
    "ANIS03D.ANI",
    "ANIS04.ANI",
    "ANIS04A.ANI",
    "ANIS04B.ANI",
    "ANIS04C.ANI",
    "ANIS05.ANI",
    "ANIS05B.ANI",
    "ANIS06.ANI",
    "ANIS07.ANI",
    "ANIS07B.ANI",
    "ANIS08.ANI",
    "ANIS09.ANI",
    "ANIT01.ANI",
    "ANIT01B.ANI",
    "ANIV01.ANI",
    "ANIV02.ANI",
    "ANIV02B.ANI",
    "ANIX01.ANI",
    "ANIX02.ANI",
    "ANIX03.ANI",
    "ANIX04.ANI",
    "ANIX05.ANI",
    "ANIX06.ANI",
    "ANIX07.ANI",
    "ANIX08.ANI",
    "ANIZ01.ANI",
    "ANIZ02.ANI",
    "ANIZ03.ANI",
    "ANIZ04.ANI",
    "ANIZ05.ANI",
    "ANIZ06.ANI",
    "ANIZ07.ANI",
    "CEL.ANI",
    "COVER.ANI",
    "LOGO.ANI",
    "PERGAM.ANI",
};

const int ani_files_count = sizeof(ani_files) / sizeof(char *);

int tr_ani_file_idx_from_string(const char *ani) {
    for (int i = 0; i < ani_files_count; i++) {
        if (strcmp(ani, ani_files[i]) == 0)
            return i;
    }

    return -1;
}

typedef struct  {
    bool inited[ani_files_count];
    tr_ani_file ani[ani_files_count];
    ray_textures textures[ani_files_count];
} ray_ani_files;

void ray_ani_files_init(ray_ani_files *ani_files) {
    memset(ani_files, 0, sizeof(ray_ani_files));
}

ray_textures *ray_ani_files_get_textures(ray_ani_files *ani_test, int ani_idx) {
    if (ani_test->inited[ani_idx]) {
        return &ani_test->textures[ani_idx];
    }

    uint8_t *ani_file = load_file(TextFormat("GAME_DIR/PLR/BNK/%s", ani_files[ani_idx]));
    tr_ani_file_init(&ani_test->ani[ani_idx], ani_file);

    tr_graphics_to_textures(&ani_test->textures[ani_idx],
                            &ani_test->ani[ani_idx].images,
                            &ani_test->ani[ani_idx].palette,
                            0);

    ani_test->inited[ani_idx] = true;
    return &ani_test->textures[ani_idx];
}

typedef struct {
    tr_palette palette;
    tr_graphics images;
} tr_ptr;

void tr_ptr_init(tr_ptr *ptr, uint8_t *data) {
    ptr->images.count = 2;
    ptr->images.items = calloc(2, sizeof(tr_image_8bpp));

    uint16_t data1   = read16_unaligned(data + 0);
    uint16_t data2   = read16_unaligned(data + 2);
    uint16_t palette = read16_unaligned(data + 4);

    tr_wdw_init_palette(&ptr->palette, data + palette);
    tr_render_ele(data + data1, &ptr->images.items[0]);
    tr_render_ele(data + data2, &ptr->images.items[1]);
}

typedef struct {
    tr_ptr ptr;
    ray_textures texts;
} dbg_ptr;

void dbg_ptr_init(dbg_ptr *dbg, const char *file) {
    uint8_t *pmouse = load_file(file);
    tr_ptr_init(&dbg->ptr, pmouse);
    tr_graphics_to_textures(&dbg->texts, &dbg->ptr.images, &dbg->ptr.palette, 0xf1);
}

void dbg_image_list_show_image(int i, Texture *texture, int scale);

void dbg_ptr_show(dbg_ptr *dbg, bool *show) {
    igSetNextWindowSize((ImVec2){500, 440}, ImGuiCond_FirstUseEver);

    igBegin("PTRs", show, 0);

    dbg_image_list_show_image(0, &dbg->texts.textures[0], 10);
    dbg_image_list_show_image(0, &dbg->texts.textures[1], 10);

    igEnd();
}

void dbg_image_list_prepare_left_pane(const char *title, bool *show) {
    igSetNextWindowSize((ImVec2){500, 440}, ImGuiCond_FirstUseEver);

    igBegin(title, show, 0);
    igBeginChild_Str("left", (ImVec2){150, 0}, true, 0);
}

bool dbg_image_list_item(const char *name, bool selected) {
    return igSelectable_Bool(name, selected, 0, zero);
}

void dbg_image_list_prepare_right_pane(const char *name, int *scale) {
    igEndChild();
    igSameLine(0, 0 );

    igBeginGroup();
    igBeginChild_Str("right", (ImVec2) {0, -igGetFrameHeightWithSpacing() }, 0, 0);

    igText("%s", name);

    igSeparator();

    igRadioButton_IntPtr("1x",  scale,  1); igSameLine(0, 1);
    igRadioButton_IntPtr("2x",  scale,  2); igSameLine(0, 1);
    igRadioButton_IntPtr("3x",  scale,  3); igSameLine(0, 1);
    igRadioButton_IntPtr("5x",  scale,  5); igSameLine(0, 1);
    igRadioButton_IntPtr("10x", scale, 10); igSameLine(0, 1);

    igSeparator();
}

void dbg_image_list_show_image(int i, Texture *texture, int scale) {
    igText("%d. %dx%d",
           i,
           texture->width,
           texture->height);

    ImVec2 ani_size = {
        texture->width  * scale,
        texture->height * scale
    };

    ImTextureID tid = &texture->id;

    igImage(tid, ani_size, zero, one, white, zero4);
    igSeparator();
}

void dbg_image_list_end(void) {
    igEndChild();

    igEndGroup();
    igEnd();
}

typedef struct {
    ray_ani_files *ani_files;
    int current;
    int scale;
} dbg_ani;

void dbg_ani_init(dbg_ani *test, ray_ani_files *ani_files) {
    memset(test, 0, sizeof(dbg_ani));
    test->ani_files = ani_files;
    test->scale = 1;
}

void dbg_ani_show(dbg_ani *test, bool *show) {
    dbg_image_list_prepare_left_pane("ANI files", show);

    for (int i = 0; i < ani_files_count; i++) {
        if (dbg_image_list_item(TextFormat("%s", ani_files[i]), test->current == i))
            test->current = i;
    }

    dbg_image_list_prepare_right_pane(ani_files[test->current], &test->scale);

    ray_ani_files_get_textures(test->ani_files, test->current);
    ray_textures *texts = &test->ani_files->textures[test->current];

    for (int i = 0; i < texts->count; i++) {
        dbg_image_list_show_image(i, &texts->textures[i], test->scale);
    }

    dbg_image_list_end();
}

void dbg_wdw_show(dbg_wdw *wdw, bool *show) {
    dbg_image_list_prepare_left_pane("WDW images", show);

    for (int i = 0; i < wdw_files_count; i++)
        if (dbg_image_list_item(wdw_files[i], wdw->current == i))
            wdw->current = i;

    dbg_image_list_prepare_right_pane("xx", &wdw->scale);

    if (!wdw->inited[wdw->current]) {
        const char *file_path = TextFormat("GAME_DIR/PLR/WDW/%s", wdw_files[wdw->current]);
        uint8_t *wdw_file = load_file(file_path);


        tr_wdw_init(&wdw->wdws[wdw->current], wdw_file);

        tr_graphics_to_textures(&wdw->textures[wdw->current],
                                &wdw->wdws[wdw->current].images,
                                &wdw->wdws[wdw->current].palette,
                                0xf1);

        wdw->inited[wdw->current] = true;
    }

    ray_textures *texts = &wdw->textures[wdw->current];
    for (int i = 0; i < texts->count; i++) {
        dbg_image_list_show_image(i, &texts->textures[i], wdw->scale);
    }

    dbg_image_list_end();
}

typedef struct {
    uint8_t first_char;
    uint8_t last_char;
    uint8_t format;
    uint8_t space_width;
    tr_graphics glyphs;
} tr_chv;

int tr_chv_get_count(tr_chv *chv) {
    return chv->last_char - chv->first_char - 1;
}

void tr_chv_init(tr_chv *chv, uint8_t *data) {
    chv->first_char  = data[0];
    chv->last_char   = data[1];
    chv->format      = data[2];
    chv->space_width = data[3];

    uint8_t *offset_start = data + 5;

    chv->glyphs.count = tr_chv_get_count(chv);
    chv->glyphs.items = calloc(chv->glyphs.count, sizeof(tr_image_8bpp));

    for (int i = 0; i < tr_chv_get_count(chv); i++) {
        uint32_t offset = read32_unaligned(offset_start + i * 4);
        uint8_t *ele_data = offset_start + offset;

        if (offset != 0xffffffff)
            tr_render_ele(ele_data, &chv->glyphs.items[i]);
    }
}

static const char *chv_files[] = {
    "GENE.CHV",
    "MANAGER.CHV",
    "HELP.CHV",
    "TXT.CHV",
    "INTRO.CHV",
};

static const int chv_files_count = sizeof(chv_files) / sizeof(char *);

typedef struct {
    tr_chv chvs[chv_files_count];
    ray_textures textures[chv_files_count];
    int current;
    int scale;
} dbg_chv;

void dbg_chv_init(dbg_chv *dbg) {
    tr_palette_file *pp = load_file("GAME_DIR/AR1/STA/ARCADE.PAL");

    tr_palette pal;
    palette_load_from_file(&pal, pp);

    dbg->scale = 1;
    dbg->current = 0;

    for (int i = 0; i < chv_files_count; i++) {
        uint8_t *data = load_file(TextFormat("GAME_DIR/FNT/%s", chv_files[i]));
        tr_chv_init(&dbg->chvs[i], data);
        tr_graphics_to_textures(&dbg->textures[i], &dbg->chvs[i].glyphs, &pal, 0x10);
    }
}

void dbg_chv_show(dbg_chv *dbg, bool *show) {
    dbg_image_list_prepare_left_pane("Fonts", show);

    for (int i = 0; i < chv_files_count; i++)
        if (dbg_image_list_item(chv_files[i], dbg->current == i))
            dbg->current = i;

    dbg_image_list_prepare_right_pane(chv_files[dbg->current], &dbg->scale);

    tr_chv *chv = &dbg->chvs[dbg->current];

    igText("first_char  %02x", chv->first_char);
    igText("last_char   %02x", chv->last_char);
    igText("format      %02x", chv->format);
    igText("space_width %02x", chv->space_width);

    ray_textures *texts = &dbg->textures[dbg->current];

    for (int i = 0; i < texts->count; i++) {
        dbg_image_list_show_image(i, &texts->textures[i], dbg->scale);
    }

    dbg_image_list_end();
}

static const char *pla_files[] = {
    "AN00.PLA",
    "AN01.PLA",
    "AN02.PLA",
    "AN03.PLA",
    "AN04.PLA",
    "AN05.PLA",
    "AN06.PLA",
    "AN07.PLA",
    "AN08.PLA",
    "AN09.PLA",
    "AN0A.PLA",
    "AN0B.PLA",
    "AN0C.PLA",
    "AN0D.PLA",
    "AN0E.PLA",
    "AN0F.PLA",
    "ANDC.PLA",
    "ANDD.PLA",
    "GAMEOVER.PLA",
    "INTRO.PLA",
    "LOGO.PLA",
};

static int pla_files_count = sizeof(pla_files) / sizeof(char*);

int tr_pla_file_idx_from_string(const char *pla) {
    for (int i = 0; i < pla_files_count; i++) {
        if (strcmp(pla, pla_files[i]) == 0)
            return i;
    }

    return -1;
}

typedef enum {
    tr_pla_args_type_16,
    tr_pla_args_type_32,
    tr_pla_args_type_string,
} tr_pla_args_type;

const int tr_pla_token_max_args = 0x10;

typedef struct {
    const char *name;
    int nargs;
    tr_pla_args_type args[tr_pla_token_max_args];
    const char *argnames[tr_pla_token_max_args];
} tr_pla_token_info;

#include "pla_token_infos.h"

typedef struct {
    uint8_t *content;
    size_t size;
    uint8_t *current;
} tr_pla_iterator;

void tr_pla_iterator_init(tr_pla_iterator *it, uint8_t *content, size_t size) {
    it->content = content;
    it->size = size;
    it->current = content;
}

size_t tr_pla_iterator_offset(tr_pla_iterator *it) {
    return it->current - it->content;
}

void tr_pla_iterator_set_offset(tr_pla_iterator *it, size_t offset) {
    it->current = it->content + offset;
}

bool tr_pla_iterator_at_end(tr_pla_iterator *it) {
    return tr_pla_iterator_offset(it) >= it->size;
}

uint16_t tr_pla_iterator_next_16(tr_pla_iterator *it) {
    uint16_t val = from_big_endian(read16_unaligned(it->current));
    it->current += 2;
    return val;
}

uint32_t tr_pla_iterator_next_32(tr_pla_iterator *it) {
    uint32_t h = tr_pla_iterator_next_16(it);
    uint32_t l = tr_pla_iterator_next_16(it);
    return (h << 16) | l;
}

const char *tr_pla_iterator_next_string(tr_pla_iterator *it) {
    const char *s = (const char *)it->current;
    uint8_t *ch = it->current;

    while (*ch != 0) {
        ch++;
        ch++;
        it->current += 2;
    }

    if ((*(ch - 1) != 0) && (read16_unaligned(it->current) == 0))
        it->current += 2;

    return s;
}

typedef enum {
    tr_player_state_ok,
    tr_player_state_stopped,
    tr_player_state_ended,
    tr_player_state_change_pla,
    tr_player_state_swivar_if_debug,
    tr_player_state_ko,
} tr_pla_player_state;

static const char *tr_player_state_strings[] = {
    "ok",
    "stopped",
    "ended",
    "change pla",
    "if debug",
    "ko",
};

typedef struct {
    tr_game *game;
    tr_resources *resources;
    tr_renderer *renderer;

    int loaded;
    uint8_t *content;
    size_t size;
    tr_pla_iterator it;

    tr_pla_player_state state;

    uint8_t *chv_data;
    tr_chv chv;

    uint32_t text_colors[2];
    int current_text_color;

    int ani_file_index;

    struct {
        int width;
        int height;
        char string[0x1000];
    } text_slots[10];

    int change_to_pla;

    size_t gosub_stack[0x10];
    int gosub_stack_count;

    struct {
        uint16_t swivar_idx;
        uint16_t kind;
        uint16_t value;
        uint16_t offset;
    } if_swivar_debug;
} tr_pla_player;

void tr_pla_player_init(tr_pla_player *player, tr_game *game, tr_resources *resources, tr_renderer *renderer) {
    memset(player, 0, sizeof(tr_pla_player));
    player->game = game;
    player->resources = resources;
    player->renderer = renderer;
}

void tr_pla_unload(tr_pla_player *player) {
    free(player->content);
    if (player->chv_data) free(player->chv_data);

    tr_pla_player_init(player, player->game, player->resources, player->renderer);
}

void tr_pla_player_load(tr_pla_player *player, int pla_idx) {
    const char *path = TextFormat("GAME_DIR/PLR/PLA/%s", pla_files[pla_idx]);

    if (player->content)
        tr_pla_unload(player);

    player->loaded = pla_idx;
    player->content = load_file_return_length(path, &player->size);
    player->state = tr_player_state_ok;
    tr_pla_iterator_init(&player->it, player->content, player->size);
}

void tr_pla_player__nop(tr_pla_player *player, const tr_pla_token_info *info) {
    for (int i = 0; i < info->nargs; i++) {
        switch (info->args[i]) {
            case tr_pla_args_type_16:
                tr_pla_iterator_next_16(&player->it);
                break;
            case tr_pla_args_type_32:
                tr_pla_iterator_next_32(&player->it);
                break;
            case tr_pla_args_type_string:
                tr_pla_iterator_next_string(&player->it);
                break;
        }
    }
}

void tr_pla_player__init(tr_pla_player *player) {
    tr_pla_player__nop(player, &pla_token_infos[0x0001]);
}

void tr_pla_player__load_ani(tr_pla_player *player) {
    const char* ani_name = tr_pla_iterator_next_string(&player->it);
    uint16_t ani_slot = tr_pla_iterator_next_16(&player->it);

    int idx = tr_ani_file_idx_from_string(ani_name);

    if (idx == -1)
        printf("bad ani %d\n", idx);
    else
        player->ani_file_index = idx;

    if (ani_slot != 0)
        printf("bad ani slot %d\n", ani_slot);
}

void tr_pla_player__render_ani(tr_pla_player *player) {
    tr_render_command *command = tr_renderer_add_command(player->renderer);
    tr_render_graphics_command *render_ani = &command->command.render_graphics;

    command->type = tr_render_command_graphics;
    render_ani->x         = tr_pla_iterator_next_16(&player->it);
    render_ani->y         = tr_pla_iterator_next_16(&player->it);
    /* unused */            tr_pla_iterator_next_16(&player->it);

    render_ani->res.type              = tr_resource_type_ani;
    render_ani->res.res.ani.frame_idx = tr_pla_iterator_next_16(&player->it);
    render_ani->res.res.ani.ani_idx   = tr_pla_iterator_next_16(&player->it);
    render_ani->flip                  = false;
    render_ani->color                 = 0;
    render_ani->palette_start         = 0;
    render_ani->center_bottom_anchor  = false;

    if (render_ani->res.res.ani.ani_idx != 0) {
        printf("bad ani idx!\n");
    }

    render_ani->res.res.ani.ani_idx = player->ani_file_index;
}

void tr_pla_player__opcode_0006(tr_pla_player *player) {
    uint16_t boh = tr_pla_iterator_next_16(&player->it);
    printf("0006 boh %04x\n", boh);
}

void tr_pla_player__delay(tr_pla_player *player) {
    /* uint16_t time     = */ tr_pla_iterator_next_16(&player->it);

    // todo
    player->state = tr_player_state_stopped;
}

void tr_pla_player__set_swivar(tr_pla_player *player) {
    uint16_t idx = tr_pla_iterator_next_16(&player->it);
    uint32_t val = tr_pla_iterator_next_32(&player->it);

    if (idx >= 0x400) {
        printf("bad swivar id %04x\n", idx);
        return;
    }

    player->game->swivar1[idx] = val;
}

void tr_pla_player__setup_animation(tr_pla_player *player) {
    uint16_t boh1        = tr_pla_iterator_next_16(&player->it);
    uint16_t boh2        = tr_pla_iterator_next_16(&player->it);
    uint32_t data_offset = tr_pla_iterator_next_32(&player->it);

    printf("setup animation %02x %02x %04x\n", boh1, boh2, data_offset);

    // todo
}

void tr_pla_player__teardown_animation(tr_pla_player *player) {
    uint16_t boh1 = tr_pla_iterator_next_16(&player->it);
    printf("teardown animation %02x\n", boh1);

    // todo
}

void tr_pla_player__fade_in(tr_pla_player *player) {
    /* uint16_t color    = */ tr_pla_iterator_next_16(&player->it);
    /* uint16_t time     = */ tr_pla_iterator_next_16(&player->it);
    /* uint16_t ani_slot = */ tr_pla_iterator_next_16(&player->it);

    // todo
    player->state = tr_player_state_stopped;
}

void tr_pla_player__fade_out(tr_pla_player *player) {
    /* uint16_t color    = */ tr_pla_iterator_next_16(&player->it);
    /* uint16_t time     = */ tr_pla_iterator_next_16(&player->it);

    // todo
    tr_renderer_clear(player->renderer);
    player->state = tr_player_state_stopped;
}

void tr_pla_player__goto_pla(tr_pla_player *player) {
    const char *pla_file = tr_pla_iterator_next_string(&player->it);

    int idx = tr_pla_file_idx_from_string(pla_file);
    if (idx == -1) {
        player->state = tr_player_state_ko;
        printf("cannot find pla %s\n", pla_file);
        return;
    }

    player->state = tr_player_state_change_pla;
    player->change_to_pla = idx;
}

void tr_pla_player__end(tr_pla_player *player) {
    player->state = tr_player_state_ended;
}

void tr_pla_player__start_music(tr_pla_player *player) {
    tr_pla_player__nop(player, &pla_token_infos[0x000b]);
}

void tr_pla_player__stop_music(tr_pla_player *player) {
    tr_pla_player__nop(player, &pla_token_infos[0x000c]);
}

void tr_pla_player__load_font(tr_pla_player *player) {
    const char *name = tr_pla_iterator_next_string(&player->it);
    printf("want font: %s\n", name);

    if (player->chv_data)
        free(player->chv_data);

    player->chv_data = load_file(TextFormat("GAME_DIR/FNT/%s", name));
    tr_chv_init(&player->chv, player->chv_data);
}

void tr_pla_player__load_mdi(tr_pla_player *player) {
    tr_pla_player__nop(player, &pla_token_infos[0x001c]);
}

void tr_pla_player__reset_swivar2(tr_pla_player *player) {
    uint16_t idx = tr_pla_iterator_next_16(&player->it);
    player->game->swivar2[idx] = 0;
}

void tr_pla_player__set_swivar2(tr_pla_player *player) {
    uint16_t idx = tr_pla_iterator_next_16(&player->it);
    player->game->swivar2[idx] = 1;
}

void tr_pla_player__load_ptr(tr_pla_player *player) {
    tr_pla_player__nop(player, &pla_token_infos[0x0023]);
}

void tr_pla_player__load_wdw(tr_pla_player *player) {
    tr_pla_player__nop(player, &pla_token_infos[0x0028]);
}

void tr_pla_player__load_texts(tr_pla_player *player) {
    tr_pla_player__nop(player, &pla_token_infos[0x0029]);
}

void tr_pla_player__append_text(tr_pla_player *player) {
    uint16_t slot = tr_pla_iterator_next_16(&player->it);
    uint16_t line = tr_pla_iterator_next_16(&player->it);

    char text[0x100];
    get_pti_line(player->resources, line, text);

    strlcat(player->text_slots[slot].string, text, sizeof(player->text_slots[slot].string));

    printf("slot line now: %s\n", player->text_slots[slot].string);
}

void tr_pla_player__render_box_text(tr_pla_player *player) {
    uint16_t slot = tr_pla_iterator_next_16(&player->it);
    /* uint16_t wdw_slot = */ tr_pla_iterator_next_16(&player->it);
    uint16_t x = tr_pla_iterator_next_16(&player->it);
    uint16_t y = tr_pla_iterator_next_16(&player->it);

    tr_render_command *command = tr_renderer_add_command(player->renderer);
    tr_render_box_text_command *render_text = &command->command.render_box_text;

    command->type = tr_render_command_type_render_box_text;
    render_text->slot   = slot;
    render_text->x      = x;
    render_text->y      = y;
    render_text->width  = player->text_slots[slot].width;
    render_text->height = player->text_slots[slot].height;
    render_text->text   = player->text_slots[slot].string;
    render_text->color  = player->text_colors[player->current_text_color];

}

void tr_pla_player__remove_text(tr_pla_player *player) {
    uint16_t slot = tr_pla_iterator_next_16(&player->it);

    player->text_slots[slot].string[0] = 0;

    tr_render_command *last = NULL;

    if (player->renderer->count)
        last = &player->renderer->commands[player->renderer->count - 1];

    if (last && last->type == tr_render_command_type_render_box_text)
        if (last->command.render_box_text.slot == slot)
            player->renderer->count--;
}

void tr_pla_player__set_text_bounds(tr_pla_player *player) {
    uint16_t slot = tr_pla_iterator_next_16(&player->it);

    player->text_slots[slot].width  = tr_pla_iterator_next_16(&player->it);
    player->text_slots[slot].height = tr_pla_iterator_next_16(&player->it);
}

void tr_pla_player__set_text_color(tr_pla_player *player) {
    uint16_t i = tr_pla_iterator_next_16(&player->it);
    uint16_t r = tr_pla_iterator_next_16(&player->it);
    uint16_t g = tr_pla_iterator_next_16(&player->it);
    uint16_t b = tr_pla_iterator_next_16(&player->it);

    if (i > sizeof(player->text_colors)) {
        printf("bad text color %d", i);
        return;
    }

    uint32_t R = r & 0xff;
    uint32_t G = g & 0xff;
    uint32_t B = b & 0xff;


    player->text_colors[i] = 0xffu |
        (R <<  8u) |
        (G << 16u) |
        (B << 24u);
}

void tr_pla_player__change_text_color(tr_pla_player *player) {
    uint16_t i = tr_pla_iterator_next_16(&player->it);

    if (i > sizeof(player->text_colors)) {
        printf("bad text color %d", i);
        return;
    }

    player->current_text_color = i;
}

void tr_pla_player__if_swivar(tr_pla_player *player) {
    uint16_t swivar_idx = tr_pla_iterator_next_16(&player->it);
    uint16_t kind       = tr_pla_iterator_next_16(&player->it);
    uint16_t value      = tr_pla_iterator_next_32(&player->it);
    uint16_t offset     = tr_pla_iterator_next_32(&player->it);

    player->state = tr_player_state_swivar_if_debug;
    player->if_swivar_debug.swivar_idx = swivar_idx;
    player->if_swivar_debug.kind = kind;
    player->if_swivar_debug.value = value;
    player->if_swivar_debug.offset = offset;
}

void tr_pla_player__gosub(tr_pla_player *player) {
    uint16_t offset = tr_pla_iterator_next_32(&player->it);
    /* uint16_t boh = */ tr_pla_iterator_next_16(&player->it);

    player->gosub_stack[player->gosub_stack_count++] = tr_pla_iterator_offset(&player->it);
    tr_pla_iterator_set_offset(&player->it, offset);
}

void tr_pla_player__return(tr_pla_player *player) {
    if (player->gosub_stack_count == 0) {
        printf("stack underflow!");
        player->state = tr_player_state_ko;
        return;
    }

    tr_pla_iterator_set_offset(&player->it, player->gosub_stack[--player->gosub_stack_count]);
}

void tr_pla_player_step(tr_pla_player *player) {
    uint32_t opcode = tr_pla_iterator_next_16(&player->it);

    switch (opcode) {
        case 0x0001: tr_pla_player__init               (player); break;
        case 0x0003: tr_pla_player__load_ani           (player); break;
        case 0x0004: tr_pla_player__render_ani         (player); break;
        case 0x0006: tr_pla_player__opcode_0006        (player); break;
        case 0x0007: tr_pla_player__delay              (player); break;
        case 0x0009: tr_pla_player__set_swivar         (player); break;
        case 0x000a: tr_pla_player__end                (player); break;
        case 0x000b: tr_pla_player__start_music        (player); break;
        case 0x000c: tr_pla_player__stop_music         (player); break;
        case 0x000e: tr_pla_player__setup_animation    (player); break;
        case 0x000f: tr_pla_player__teardown_animation (player); break;
        case 0x0010: tr_pla_player__fade_in            (player); break;
        case 0x0011: tr_pla_player__fade_out           (player); break;
        case 0x0013: tr_pla_player__goto_pla           (player); break;
        case 0x0019: tr_pla_player__load_font          (player); break;
        case 0x001c: tr_pla_player__load_mdi           (player); break;
        case 0x001f: tr_pla_player__reset_swivar2      (player); break;
        case 0x0020: tr_pla_player__set_swivar2        (player); break;
        case 0x0023: tr_pla_player__load_ptr           (player); break;
        case 0x0028: tr_pla_player__load_wdw           (player); break;
        case 0x0029: tr_pla_player__load_texts         (player); break;
        case 0x002a: tr_pla_player__append_text        (player); break;
        case 0x002c: tr_pla_player__render_box_text    (player); break;
        case 0x002d: tr_pla_player__remove_text        (player); break;
        case 0x002e: tr_pla_player__set_text_bounds    (player); break;
        case 0x0031: tr_pla_player__set_text_color     (player); break;
        case 0x0032: tr_pla_player__change_text_color  (player); break;
        case 0x0033: tr_pla_player__if_swivar          (player); break;
        case 0x0035: tr_pla_player__gosub              (player); break;
        case 0x0036: tr_pla_player__return             (player); break;

        default:
            player->state = tr_player_state_ko;
            printf("cant op-code %04x\n", opcode);

            player->it.current -= 2;
            break;
    }
}

void tr_pla_player_resume(tr_pla_player *player) {
    if (player->state == tr_player_state_stopped)
        player->state = tr_player_state_ok;

    while (!tr_pla_iterator_at_end(&player->it) && player->state == tr_player_state_ok) {
        printf("playing: %lx\n", tr_pla_iterator_offset(&player->it));
        tr_pla_player_step(player);
    }

    if (player->state == tr_player_state_change_pla) {
        tr_pla_player_load(player, player->change_to_pla);
    }
}

typedef struct {
    tr_pla_player *player;

    int selected;

    bool show_script_offset;
    bool show_script_opcode;
    bool show_script_arg_names;
} dbg_pla;

void dbg_pla_init(dbg_pla *pla, tr_pla_player *player) {
    pla->player = player;

    pla->show_script_offset = true;
    pla->show_script_opcode = true;
    pla->show_script_arg_names = true;
}

void dbg_pla_dump(uint8_t *content, size_t size, size_t ip, bool show_offset, bool show_opcode, bool show_arg_names) {
    tr_pla_iterator it;

    tr_pla_iterator_init(&it, content, size);

    while (!tr_pla_iterator_at_end(&it))
    {
        size_t offset = tr_pla_iterator_offset(&it);

        if (offset == ip)
            igText("-> ");
        else
            igText("   ");

        igSameLine(0, 0);

        if (show_offset) {
            igPushStyleColor_U32(ImGuiCol_Text, 0xffaaaaaa);
            igText("%5x ", offset);
            igSameLine(0, 0);
            igPopStyleColor(1);
        }

        uint16_t opcode = tr_pla_iterator_next_16(&it);

        if (show_opcode) {
            igPushStyleColor_U32(ImGuiCol_Text, 0xffcccccc);
            igText("(%04x) ", opcode);
            igSameLine(0, 0);
            igPopStyleColor(1);
        }

        if (opcode > pla_token_infos_count) {
            igText("bad opcode: %04x", opcode);
            break;
        }

        tr_pla_token_info info = pla_token_infos[opcode];

        if (pla_token_infos[opcode].name)
            igText("%s: ", pla_token_infos[opcode].name);
        else
            igText("opcode_%04x: ", opcode);

        igSameLine(0, 1);

        for (int a = 0; a < info.nargs; a++) {
            if (info.argnames[a] != NULL && show_arg_names) {
                igPushStyleColor_U32(ImGuiCol_Text, 0xff555555);
                igText("%s: ", info.argnames[a]); igSameLine(0, 1);
                igPopStyleColor(1);
            }

            switch (info.args[a]) {
                case tr_pla_args_type_16:
                    igText("%02x ", tr_pla_iterator_next_16(&it));
                    igSameLine(0, 0);
                    break;

                case tr_pla_args_type_32:
                    igText("%04x ", tr_pla_iterator_next_32(&it));
                    igSameLine(0, 0);
                    break;

                case tr_pla_args_type_string: {
                    igText("%s ", tr_pla_iterator_next_string(&it));
                    igSameLine(0, 0);
                    break;
                }
            }
        }

        igNewLine();
    }
}

void dbg_pla_show_load(dbg_pla *pla, bool *show) {
    igSetNextWindowSize((ImVec2){600, 0}, ImGuiCond_FirstUseEver);

    igBegin("Load", show, 0);
    igCombo_Str_arr("pla", &pla->selected, pla_files, pla_files_count, 0);

    if (igButton("load", zero)) {
        tr_pla_player_load(pla->player, pla->selected);
        *show = false;
    }

    igEnd();
}

void dbg_pla_show_script(dbg_pla *pla, bool *show) {
    igSetNextWindowSize((ImVec2){600, 300}, ImGuiCond_FirstUseEver);

    igBegin("Current PLA", show, 0);

    igText("loaded: %s", pla_files[pla->player->loaded]);

    igCheckbox("offsets", &pla->show_script_offset); igSameLine(0, -1);
    igCheckbox("opcodes", &pla->show_script_opcode); igSameLine(0, -1);
    igCheckbox("args", &pla->show_script_arg_names);

    igSeparator();

    igBeginChild_Str("scri", zero, 0, 0);

    dbg_pla_dump(pla->player->content,
                 pla->player->size,
                 tr_pla_iterator_offset(&pla->player->it),
                 pla->show_script_offset,
                 pla->show_script_opcode,
                 pla->show_script_arg_names);

    igEndChild();
    igEnd();
}

typedef struct {
    ray_textures status_tex;
    ray_textures numeri_tex;
    ray_textures k_tex;
    ray_textures tr_tex;
    ray_textures ucci0_tex;
    ray_textures ucci1_tex;

    ray_textures *ele_textures[tr_ele_count];

    ray_ani_files ani_files;

    ray_bg_renderer bg_renderer;

    int rendered_room;
    bool show_types;
    int show_arcade;

    bool did_autoplay;
    bool running;
    int resume_countdown;
    bool open_script;
} ray_gameloop;

void ray_gameloop_init(ray_gameloop *ray_loop, tr_gameloop *tr_loop) {
    tr_graphics_to_textures(&ray_loop->status_tex, &tr_loop->status_ele, &tr_loop->palette, 0xc1);
    tr_graphics_to_textures(&ray_loop->numeri_tex, &tr_loop->numeri_ele, &tr_loop->palette, 0xc1);
    tr_graphics_to_textures(&ray_loop->k_tex,      &tr_loop->k_ele,      &tr_loop->palette, 0xc1);
    tr_graphics_to_textures(&ray_loop->tr_tex,     &tr_loop->tr_ele,     &tr_loop->palette, 0xc1);
    tr_graphics_to_textures(&ray_loop->ucci0_tex,  &tr_loop->ucci0_ele,  &tr_loop->palette, 0xc1);
    tr_graphics_to_textures(&ray_loop->ucci1_tex,  &tr_loop->ucci1_ele,  &tr_loop->palette, 0xc1);

    ray_loop->ele_textures[tr_ele_status] = &ray_loop->status_tex;
    ray_loop->ele_textures[tr_ele_numeri] = &ray_loop->numeri_tex;
    ray_loop->ele_textures[tr_ele_k]      = &ray_loop->k_tex;
    ray_loop->ele_textures[tr_ele_tr]     = &ray_loop->tr_tex;
    ray_loop->ele_textures[tr_ele_ucci0]  = &ray_loop->ucci0_tex;
    ray_loop->ele_textures[tr_ele_ucci1]  = &ray_loop->ucci1_tex;

    ray_ani_files_init(&ray_loop->ani_files);

    ray_bg_renderer_init(&ray_loop->bg_renderer,
                         &tr_loop->resources,
                         &tr_loop->tilesets,
                         &tr_loop->palette);

    ray_bg_render_room(&ray_loop->bg_renderer, 0, 0);

    ray_loop->show_arcade = false;
    ray_loop->rendered_room = 0;
    ray_loop->show_types  = true;
    ray_loop->did_autoplay = false;
    ray_loop->open_script = false;
    ray_loop->resume_countdown = 0;
}

void ray_gameloop_update(ray_gameloop *ray_loop, tr_gameloop *tr_loop) {
    if (IsKeyPressed(KEY_U)) {
        ray_loop->show_types = !ray_loop->show_types;
    }

    if (IsKeyPressed(KEY_R)) {
        tr_game_reset(&tr_loop->game, &tr_loop->resources);
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        tr_loop->game.x = tr_loop->game.to_set_x;
        tr_loop->game.y = tr_loop->game.to_set_y;
    }

    if (IsKeyPressed(KEY_PAGE_DOWN)) {
        tr_loop->game.current_room = tr_loop->game.current_room + 1;
    }

    if (IsKeyPressed(KEY_PAGE_UP)) {
        tr_loop->game.current_room = tr_loop->game.current_room - 1;
    }
}

void ray_arcade_gameloop_tick(ray_gameloop *ray_loop, tr_gameloop *tr_loop, tr_renderer *renderer) {
    char direction = tr_keys_to_direction(
        IsKeyDown(KEY_DOWN),
        IsKeyDown(KEY_UP),
        IsKeyDown(KEY_LEFT),
        IsKeyDown(KEY_RIGHT),
        IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)
    );

    tr_arcade_gameloop_tick(renderer, &tr_loop->game, &tr_loop->resources, direction);

    bool redraw_bg = bg_step(&tr_loop->bg);

    if ((ray_loop->rendered_room != tr_loop->game.current_room) || redraw_bg) {
        ray_bg_render_room(&ray_loop->bg_renderer, tr_loop->game.current_room, tr_loop->bg.frame);
        ray_loop->rendered_room = tr_loop->game.current_room;
    }

    if (ray_loop->show_types) {
        tr_render_command *cmd = tr_renderer_add_command(renderer);
        cmd->type = tr_render_command_arcade_types;
    }
}

typedef struct {
    struct ImGuiIO *io;
    Texture font_texture;
    bool show_imgui_demo;

    tr_pla_player *player;

    dbg_ani ani;
    bool show_ani;

    dbg_wdw wdw;
    bool show_wdw;

    dbg_ptr ptr;
    bool show_ptr;

    dbg_chv chv;
    bool show_chv;

    dbg_pla pla;
    bool show_load_pla;
    bool show_script;
} dbg_ui;

void dbg_ui_init(dbg_ui *ui, tr_game *game, tr_resources *resources, tr_pla_player *player, ray_gameloop *ray_loop) {
    igCreateContext(NULL);
    igStyleColorsDark(NULL);

    ImGui_ImplRaylib_Init();

    ui->io = igGetIO();
    ui->io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ui->io->LogFilename = NULL;
    ui->io->IniFilename = NULL;

    Image font_image;
    font_image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    font_image.mipmaps = 1;

    ImFontAtlas_GetTexDataAsRGBA32(ui->io->Fonts,
                                   (uint8_t **)&font_image.data,
                                   &font_image.width,
                                   &font_image.height,
                                   NULL);

    ui->font_texture = LoadTextureFromImage(font_image);
    ui->io->Fonts->TexID = (ImTextureID *)(&ui->font_texture.id);

    ui->show_imgui_demo = false;

    ui->show_ani = false;
    dbg_ani_init(&ui->ani, &ray_loop->ani_files);

    ui->show_wdw = false;
    dbg_wdw_init(&ui->wdw);

    ui->show_ptr = false;
    dbg_ptr_init(&ui->ptr, "GAME_DIR/PLR/WDW/PMOUSE.I16");

    ui->show_chv = false;
    dbg_chv_init(&ui->chv);

    ui->show_load_pla = false;
    ui->show_script = false;
    ui->player = player;
    dbg_pla_init(&ui->pla, ui->player);
}

void dbg_ui_props(dbg_ui *ui, ray_gameloop *ray_loop, tr_gameloop *tr_loop) {
    igInputInt("room nr", &tr_loop->game.current_room, 1, 1, ImGuiInputTextFlags_CharsHexadecimal);
    igLabelText("pupo",         "%4x %4x", tr_loop->game.x, tr_loop->game.y);
    igNewLine();
    igLabelText("ani",          "%02x",    tr_loop->game.ani);
    igLabelText("frame",        "%02x",    tr_loop->game.frame_nr);
    igLabelText("ctd",          "%2x",     tr_loop->game.countdown);
    igLabelText("room",         "%2x",     tr_loop->game.current_room);
    igLabelText("to_set_x",     "%2x",     tr_loop->game.to_set_x);
    igLabelText("to_set_y",     "%2x",     tr_loop->game.to_set_y);
    igLabelText("count caduta", "%2x",     tr_loop->game.counter_caduta);
}

void dbg_ui_hint_lines(dbg_ui *ui, ray_gameloop *ray_loop, tr_gameloop *tr_loop) {
    uint16_t line1, line2;
    get_explanation(&tr_loop->game, &tr_loop->resources, &line1, &line2);

    char line1_str[0x100];
    char line2_str[0x100];

    line1_str[0] = 0;
    line2_str[0] = 0;

    get_pti_line(&tr_loop->resources, line1, line1_str);
    get_pti_line(&tr_loop->resources, line2, line2_str);

    igText("%s", line1_str);
    igText("%s", line2_str);
}

void dbg_ui_pla_controls(dbg_ui *ui) {
    dbg_pla *pla = &ui->pla;

    igText("loaded:  %s", pla_files[pla->player->loaded]);
    igText("current: %05x", tr_pla_iterator_offset(&pla->player->it));
    igText("state:   %s", tr_player_state_strings[pla->player->state]);
    igText("stack:   %d", pla->player->gosub_stack_count);

    igSeparator();

    if (igButton("show script", zero)) {
        ui->show_script = !ui->show_script;
    }

    igSameLine(0, -1);

    if (igButton("load script", zero)) {
        ui->show_load_pla = !ui->show_load_pla;
    }

    igSeparator();

    if (igButton("step", zero)) {
        tr_pla_player_step(pla->player);
    }

    igSameLine(0, -1);

    if (igButton("resume", zero)) {
        tr_pla_player_resume(pla->player);

    }
}

void dbg_if_swivar(dbg_ui *ui, ray_gameloop *ray_loop, tr_gameloop *tr_loop) {
    if (ui->player->state == tr_player_state_swivar_if_debug) {
        ui->show_script = true;

        igBegin("if-swivar debug", NULL, 0);
        igText("if swivar opcode:");
        igText(TextFormat("swivar idx: %02x", ui->player->if_swivar_debug.swivar_idx));
        igText(TextFormat("kind:       %02x", ui->player->if_swivar_debug.kind));
        igText(TextFormat("value:      %04x", ui->player->if_swivar_debug.value));
        igText(TextFormat("offset:     %04x", ui->player->if_swivar_debug.offset));

        if (igButton("do it", zero)) {
            ui->player->it.current = ui->player->it.content + ui->player->if_swivar_debug.offset;
            ui->player->state = tr_player_state_ok;
        }

        if (igButton("dont do it", zero)) {
            ui->player->state = tr_player_state_ok;
        }

        igEnd();
    }
}

void dbg_ui_update(dbg_ui *ui, ray_gameloop *ray_loop, tr_gameloop *tr_loop) {
    ImGui_ImplRaylib_NewFrame();
    ImGui_ImplRaylib_ProcessEvent();

    igNewFrame();

    ImVec2 position = { GAME_SIZE_WIDTH * GAME_SIZE_SCALE, 0 };
    ImVec2 size = { DEBUG_PANE_WIDTH, GAME_SIZE_HEIGHT * GAME_SIZE_SCALE };

    igSetNextWindowPos(position, 0, zero);
    igSetNextWindowSize(size, 0);

    int main_window_flags = 0       |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove     |
        ImGuiWindowFlags_NoCollapse;

    igBegin("main", NULL, main_window_flags);

    if (igButton("ani", zero)) ui->show_ani = !ui->show_ani; igSameLine(0, -1);
    if (igButton("wdw", zero)) ui->show_wdw = !ui->show_wdw; igSameLine(0, -1);
    if (igButton("ptr", zero)) ui->show_ptr = !ui->show_ptr; igSameLine(0, -1);
    if (igButton("chv", zero)) ui->show_chv = !ui->show_chv; igSameLine(0, -1);

    if (igButton("imgui demo", zero)) ui->show_imgui_demo = !ui->show_imgui_demo;

    igNewLine();

    bool change_game_mode = false;

    if (igRadioButton_IntPtr("arcade", &ray_loop->show_arcade, 1))
        change_game_mode = true;

    if (igRadioButton_IntPtr("player", &ray_loop->show_arcade, 0))
        change_game_mode = true;

    igNewLine();
    igSeparator();
    igNewLine();

    if (change_game_mode)
        igSetNextItemOpen(ray_loop->show_arcade, 0);

    if (igCollapsingHeader_BoolPtr("arcade debug", NULL, 0)) {
        dbg_ui_props(ui, ray_loop, tr_loop);
        igNewLine();

        dbg_ui_hint_lines(ui, ray_loop, tr_loop);
        igNewLine();

        igCheckbox("show tile types", &ray_loop->show_types);

        if (igButton("Reset", zero)) {
            tr_game_reset(&tr_loop->game, &tr_loop->resources);
        }

        if (igButton("Room pos", zero)) {
            tr_loop->game.x = tr_loop->game.to_set_x;
            tr_loop->game.y = tr_loop->game.to_set_y;
        }
    }

    if (change_game_mode)
        igSetNextItemOpen(!ray_loop->show_arcade, 0);

    if (igCollapsingHeader_BoolPtr("player debug", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
        dbg_ui_pla_controls(ui);
    }

    igEnd();

    dbg_if_swivar(ui, ray_loop, tr_loop);

    if (ui->show_imgui_demo) igShowDemoWindow(NULL);

    if (ui->show_ani) dbg_ani_show(&ui->ani, &ui->show_ani);
    if (ui->show_wdw) dbg_wdw_show(&ui->wdw, &ui->show_wdw);
    if (ui->show_ptr) dbg_ptr_show(&ui->ptr, &ui->show_ptr);
    if (ui->show_chv) dbg_chv_show(&ui->chv, &ui->show_chv);
    if (ui->show_load_pla) dbg_pla_show_load(&ui->pla, &ui->show_load_pla);

    if (ray_loop->open_script) {
        ui->show_script = true;
        ray_loop->open_script = false;
    }
    
    if (ui->show_script) dbg_pla_show_script(&ui->pla, &ui->show_script);
}

void dbg_ui_render(void) {
    igRender();
    raylib_render_cimgui(igGetDrawData());
    igUpdatePlatformWindows();
}

typedef struct  {
    float elapsed;
    float frame_time;
    bool  do_frame;
} ray_framerate;

void ray_framerate_init(ray_framerate *framerate, int fps) {
    framerate->elapsed    = 0;
    framerate->frame_time = 1/((float) fps);
    framerate->do_frame   = false;
}

void ray_framerate_update(ray_framerate *framerate) {
    framerate->do_frame = false;
    framerate->elapsed += GetFrameTime();

    if (framerate->elapsed > framerate->frame_time) {
        framerate->do_frame = true;
        framerate->elapsed -= framerate->frame_time;
    }
}

bool ray_framerate_do_frame(ray_framerate *framerate) {
    return framerate->do_frame;
}

Texture ray_texture_for_resource(tr_graphics_resource resource, ray_gameloop *ray_loop) {
    switch (resource.type) {
        case tr_resource_type_ele: {
            return ray_loop
                ->ele_textures[resource.res.ele.ele_kind]
                ->textures[resource.res.ele.ele_idx];
        }
        case tr_resource_type_ani: {
            return ray_ani_files_get_textures(&ray_loop->ani_files,
                                              resource.res.ani.ani_idx)
                ->textures[resource.res.ani.frame_idx];
        }
    }
}

void ray_renderer_draw(ray_gameloop *ray_loop, tr_gameloop *tr_loop, tr_pla_player *player, tr_renderer *renderer) {

    for (int i = 0; i < renderer->count; i++) {
        tr_render_command *cmd = &renderer->commands[i];
        switch (cmd->type) {
            case tr_render_command_graphics: {
                tr_render_graphics_command *g = &cmd->command.render_graphics;
                Texture texture = ray_texture_for_resource(g->res, ray_loop);

                int delta_x = !g->center_bottom_anchor ? 0 : texture.width / 2;
                int delta_y = !g->center_bottom_anchor ? 0 : texture.height;

                DrawTextureScaled(texture,
                                  g->x - delta_x,
                                  g->y - delta_y,
                                  texture.width,
                                  texture.height,
                                  g->flip);
                break;
            }

            case tr_render_command_type_render_box_text: {
                tr_render_box_text_command *text_cmd = &cmd->command.render_box_text;
                DrawRectangleLines(text_cmd->x * GAME_SIZE_SCALE,
                                   text_cmd->y * GAME_SIZE_SCALE,
                                   text_cmd->width * GAME_SIZE_SCALE,
                                   text_cmd->y * GAME_SIZE_SCALE,
                                   PINK);
                DrawText(text_cmd->text,
                         text_cmd->x * GAME_SIZE_SCALE,
                         text_cmd->y * GAME_SIZE_SCALE,
                         20,
                         GetColor(text_cmd->color));
                break;
            }

            case tr_render_command_arcade_background: {
                DrawTextureScaled(ray_loop->bg_renderer.texture,
                                  0,
                                  0,
                                  GAME_SIZE_WIDTH,
                                  GAME_SIZE_HEIGHT,
                                  false);
                break;
            }

            case tr_render_command_arcade_types: {
                DrawTileTypes(&tr_loop->resources, tr_loop->game.current_room);
                break;
            }
        }
    }

    if (!ray_loop->show_arcade &&
        ray_loop->resume_countdown &&
        player->state == tr_player_state_stopped)
    {
        DrawText(TextFormat("%d", ray_loop->resume_countdown), 0, 0, 40, ORANGE);
    }
}

void ray_player_gameloop_tick(ray_gameloop *ray_loop, tr_gameloop *tr_loop, tr_pla_player *player) {
    if ((player->state == tr_player_state_ok) && !ray_loop->did_autoplay) {
        tr_pla_player_resume(player);
        ray_loop->resume_countdown = 20;
        ray_loop->running = true;
    }

    if (!ray_loop->running)
        return;

    if (player->state == tr_player_state_stopped) {
        if (ray_loop->resume_countdown-- == 0) {
            tr_pla_player_resume(player);
            ray_loop->resume_countdown = 20;
        }
    }
    else {
        ray_loop->running = false;
    }
}

int main() {
    InitWindow(GAME_SIZE_WIDTH  * GAME_SIZE_SCALE + DEBUG_PANE_WIDTH,
               GAME_SIZE_HEIGHT * GAME_SIZE_SCALE,
               "Corridori");

    SetWindowPosition(2000, 200);
    SetTargetFPS(60);

    tr_gameloop tr_loop;
    tr_gameloop_init(&tr_loop);

    tr_renderer renderer;
    tr_renderer_clear(&renderer);

    tr_pla_player player;
    tr_pla_player_init(&player, &tr_loop.game, &tr_loop.resources, &renderer);
    tr_pla_player_load(&player, 0);

    ray_gameloop ray_loop;
    ray_gameloop_init(&ray_loop, &tr_loop);

    ray_framerate framerate;
    ray_framerate_init(&framerate, 20);

    dbg_ui ui;
    dbg_ui_init(&ui, &tr_loop.game, &tr_loop.resources, &player, &ray_loop);

    while (!WindowShouldClose()) {
        dbg_ui_update(&ui, &ray_loop, &tr_loop);
        ray_gameloop_update(&ray_loop, &tr_loop);

        if (ray_framerate_do_frame(&framerate)) {
            if (ray_loop.show_arcade) {
                ray_arcade_gameloop_tick(&ray_loop, &tr_loop, &renderer);
            }
            else {
                ray_player_gameloop_tick(&ray_loop, &tr_loop, &player);
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        ray_renderer_draw(&ray_loop, &tr_loop, &player, &renderer);

        dbg_ui_render();

        EndDrawing();

        ray_framerate_update(&framerate);
    }

    CloseWindow();

    return 0;
}
