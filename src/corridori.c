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

uint16_t room_get_tile_id(uint8_t *room_file, int room, int pos) {
    int offset = (room * 0x04f4) + 0x0024 + (pos * 2);
    return from_big_endian(*(uint16_t *)(room_file + offset));
}

uint8_t room_get_mat_file(uint8_t *room_file, int room, int pos) {
    int offset = (room * 0x04f4) + 0x0004 + (pos * 2) + 1;
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

void tr_graphics_init(tr_graphics *graphics, uint8_t *ele_file, int col) {
    graphics->count = (*(uint16_t *)ele_file) - 1;
    graphics->items = calloc(graphics->count, sizeof(tr_image_8bpp));

    for (int i = 0; i < graphics->count; i++)
    {
        int           offset    = (i * 4) + 2;
        uint8_t       *src_offset = ele_file + offset;
        uint8_t       *src_item = ele_file + 2 + read32_unaligned(src_offset);

        tr_image_8bpp *dst_item = &graphics->items[i];

        printf("\n\nELE %02x\n\n", i);

        dst_item->width  = read16_unaligned(src_item + 0 * 2);
        dst_item->height = read16_unaligned(src_item + 1 * 2);

        int size = dst_item->width * dst_item->height;
        dst_item->pixels = malloc(size);
        dst_item->mask   = malloc(size);

        memset(dst_item->pixels, 0, size);
        memset(dst_item->mask,   0, size);

        uint8_t *end = dst_item->pixels + (size);

        int y = 0;

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

                    for (int i = 0; i < skip; i++) printf("   ");

                    uint8_t count = *src++;
                    if (count != 0xff) {
                        consecutive_ffs = 0;

                        for (int i = 0; i < count / 2; i++) {
                            uint8_t colors = *src++;
                            uint8_t color1 = ((colors & 0x0f)     ) + col;
                            uint8_t color2 = ((colors & 0xf0) >> 4) + col;

                            printf("%02x ", color1);
                            printf("%02x ", color2);

                            if (dst < end) {
                                *dst++ = color1; *msk++ = 1;
                                *dst++ = color2; *msk++ = 1;
                            }
                            else {
                                printf("DIOCAN");
                            }
                            x_left -= 2;
                        }

                        if (count & 1) {
                            uint color = *src++;
                            uint color1 = (color & 0x0f) + col;

                            printf("%02x ", color1);

                            if (dst < end) {
                                *dst++ = color1; *msk++ = 1;
                            }
                            else {
                                printf("DIOCAN");
                            }

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
                    for (int i = 0; i < x_left; i++) printf("   ");

                    printf("|  line: %x", y);

                    x_left = dst_item->width;

                    printf("\n");
                    y++;

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

int main() {
    InitWindow(GAME_SIZE_WIDTH  * GAME_SIZE_SCALE,
               GAME_SIZE_HEIGHT * GAME_SIZE_SCALE,
               "Corridori");

    SetTargetFPS(30);

    tr_resources resources;
    tr_palette   palette;
    tr_tilesets  tilesets;

    resources_load(&resources);

    palette_init(&palette, &resources);
    tilesets_init(&tilesets, &resources);

    uint32_t data[GAME_SIZE_WIDTH * GAME_SIZE_HEIGHT];
    uint8_t background[GAME_SIZE_WIDTH * GAME_SIZE_HEIGHT];

    render_background_layer(resources.room_roe, 0, &tilesets, background);

    for (int i = 0; i < sizeof(data) / sizeof(uint32_t); i++) {
        data[i] = palette.color[background[i]];
    }

    tr_graphics tr;
    tr_graphics_init(&tr, resources.tr_ele, 0);

    Texture2D *tr_ele_textures = calloc(tr.count, sizeof(Texture2D));
    uint32_t **tr_ele_image_data = calloc(tr.count, sizeof(uint32_t*));

    for (int image = 0; image < tr.count; image++) {
        tr_image_8bpp *img = &tr.items[image];
        tr_ele_image_data[image] = malloc(img->width * img->height * sizeof(uint32_t));

        for (int pix = 0; pix < img->width * img->height; pix++) {
            uint8_t color = img->pixels[pix];
            uint8_t mask  = img->mask  [pix];

            tr_ele_image_data[image][pix] = mask == 0 ? 0 : palette.color[color + 0xc1];
        }

        tr_ele_textures[image] = LoadTextureFromImage((Image) {
            .data = tr_ele_image_data[image],
            .width = img->width,
            .height = img->height,
            .format = UNCOMPRESSED_R8G8B8A8,
            .mipmaps = 1,
        });

        SetTextureFilter(tr_ele_textures[image], FILTER_POINT);
    }

    Texture2D room_texture = LoadTextureFromImage((Image) {
        .data = &data,
        .width = GAME_SIZE_WIDTH,
        .height = GAME_SIZE_HEIGHT,
        .format = UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1,
    });

    SetTextureFilter(room_texture, FILTER_POINT);

    int the_room = 0;
    int the_ele = 0;
    int old_room = the_room;
    int old_ele = the_ele;

    Texture2D *ele_texture = &tr_ele_textures[the_ele];

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyPressed(KEY_RIGHT))  { the_room += 1; }
        if (IsKeyPressed(KEY_LEFT))   { the_room -= 1; }

        if (IsKeyPressed(KEY_UP))     { the_ele += 1; }
        if (IsKeyPressed(KEY_DOWN))   { the_ele -= 1; }

        the_room = MAX(MIN(the_room, 0x2b), 0);
        the_ele  = MAX(MIN(the_ele, tr.count), 0);

        if (the_room != old_room) {
            old_room = the_room;

            render_background_layer(resources.room_roe, the_room, &tilesets, background);

            for (int i = 0; i < sizeof(data) / sizeof(uint32_t); i++) {
                data[i] = palette.color[background[i]];
            }

            UpdateTexture(room_texture, data);
        }

        if (the_ele != old_ele) {
            ele_texture = &tr_ele_textures[the_ele];
            old_ele = the_ele;
        }

        DrawTextureScaled(room_texture, 0, 0, GAME_SIZE_WIDTH, GAME_SIZE_HEIGHT);
        DrawTextureScaled(*ele_texture, 20, 20, tr.items[the_ele].width, tr.items[the_ele].height);


        char suca[0x100];
        sprintf(suca, "ROOM %2x", the_room);
        DrawText(suca, 20, 20, 30, PURPLE);
        sprintf(suca, "ELE  %2x", the_ele);
        DrawText(suca, 20, 50, 30, PURPLE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
