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
} tr_resources;

// - //

typedef struct {
    int count;
    Texture2D *textures;
    uint32_t  **data;
} ray_textures;

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

#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)

void DrawTextureScaled(Texture texture, int x, int y, int width, int height) {
    Rectangle sourceRect = {
        .x = 0,
        .y = 0,
        .width = width,
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

int main() {
    InitWindow(GAME_SIZE_WIDTH  * GAME_SIZE_SCALE,
               GAME_SIZE_HEIGHT * GAME_SIZE_SCALE,
               "Corridori");

    SetTargetFPS(30);

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

    uint32_t data[GAME_SIZE_WIDTH * GAME_SIZE_HEIGHT];
    uint8_t background[GAME_SIZE_WIDTH * GAME_SIZE_HEIGHT];

    render_background_layer(resources.room_roe, 0, &tilesets, background);

    for (int i = 0; i < sizeof(data) / sizeof(uint32_t); i++) {
        data[i] = palette.color[background[i]];
    }

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

    ray_textures *texts[] = {
        &status_tex,
        &numeri_tex,
        &k_tex,
        &tr_tex,
        &ucci0_tex,
        &ucci1_tex,
    };

    Texture2D room_texture = LoadTextureFromImage((Image) {
        .data = &data,
        .width = GAME_SIZE_WIDTH,
        .height = GAME_SIZE_HEIGHT,
        .format = UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1,
    });

    SetTextureFilter(room_texture, FILTER_POINT);

    int the_room = 0;
    int the_ele  = 0;
    int the_item = 0;

    int old_room = the_room;
    int old_ele  = the_ele;
    int old_item = the_item;

    bool show_types = false;

    Texture2D *test_texture = &texts[the_ele]->textures[the_item];

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyPressed(KEY_RIGHT))     { the_room += 1; }
        if (IsKeyPressed(KEY_LEFT))      { the_room -= 1; }

        if (IsKeyPressed(KEY_PAGE_UP))   { the_ele  += 1; }
        if (IsKeyPressed(KEY_PAGE_DOWN)) { the_ele  -= 1; }

        if (IsKeyPressed(KEY_UP))        { the_item += 1; }
        if (IsKeyPressed(KEY_DOWN))      { the_item -= 1; }

        if (IsKeyPressed(KEY_M))         { show_types = !show_types; }

        char direction = tr_keys_to_direction(
          IsKeyDown(KEY_DOWN),
          IsKeyDown(KEY_UP),
          IsKeyDown(KEY_LEFT),
          IsKeyDown(KEY_RIGHT),
          IsKeyDown(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)
        );

        the_room = MAX(MIN(the_room, 0x2b  - 1), 0);
        the_ele  = MAX(MIN(the_ele, sizeof(texts) / sizeof(ray_textures *) - 1), 0);
        the_item = MAX(MIN(the_item, texts[the_ele]->count - 1), 0);

        if (the_room != old_room) {
            old_room = the_room;

            render_background_layer(resources.room_roe, the_room, &tilesets, background);

            for (int i = 0; i < sizeof(data) / sizeof(uint32_t); i++) {
                data[i] = palette.color[background[i]];
            }

            UpdateTexture(room_texture, data);
        }

        if (the_item != old_item) {
            test_texture = &texts[the_ele]->textures[the_item];
            old_item = the_item;
        }

        if (the_ele != old_ele) {
            the_item = 0;

            test_texture = &texts[the_ele]->textures[the_item];
            old_ele = the_ele;
            old_item = 0;
        }

        DrawTextureScaled(room_texture, 0, 0, GAME_SIZE_WIDTH, GAME_SIZE_HEIGHT);
        DrawTextureScaled(*test_texture, 100, 50, test_texture->width, test_texture->height);

        char suca[0x100];

        if (show_types) {
            DrawTileTypes(&resources, the_room);
        }
        else {
            sprintf(suca, "ROOM %2x", the_room);
            DrawText(suca, 20, 20, 20, PURPLE);
            sprintf(suca, "ELE  %2x", the_ele);
            DrawText(suca, 20, 40, 20, PURPLE);
            sprintf(suca, "ITEM %2x", the_item);
            DrawText(suca, 20, 60, 20, PURPLE);
        }

        sprintf(suca, "dir: %2x", direction);
        DrawText(suca, 20, 0, 20, GREEN);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
