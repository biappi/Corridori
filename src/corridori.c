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

#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)

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

    Image image = {
        .data = &data,
        .width = GAME_SIZE_WIDTH,
        .height = GAME_SIZE_HEIGHT,
        .format = UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1,
    };

    for (int i = 0; i < sizeof(data) / sizeof(uint32_t); i++) {
        data[i] = palette.color[background[i]];
    }

    Texture2D texture = LoadTextureFromImage(image);
    SetTextureFilter(texture, FILTER_POINT);

    int selection = 0;
    int oldselection = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyPressed(KEY_RIGHT))  { selection += 1; }
        if (IsKeyPressed(KEY_LEFT))   { selection -= 1; }

        selection = MAX(MIN(selection, 0x2b), 0);

        if (selection != oldselection) {
            oldselection = selection;

            render_background_layer(resources.room_roe, selection, &tilesets, background);

            for (int i = 0; i < sizeof(data) / sizeof(uint32_t); i++) {
                data[i] = palette.color[background[i]];
            }

            UpdateTexture(texture, data);
        }


        Rectangle sourceRect = {
            .x = 0,
            .y = 0,
            .width = GAME_SIZE_WIDTH,
            .height = GAME_SIZE_HEIGHT
        };

        Rectangle destRect = {
            .x = 0,
            .y = 0,
            .width = GAME_SIZE_WIDTH  * GAME_SIZE_SCALE,
            .height = GAME_SIZE_HEIGHT * GAME_SIZE_SCALE
        };

        DrawTexturePro(texture, sourceRect, destRect, (Vector2) { .x  = 0, .y = 0 }, 0, WHITE);


        char suca[0x100];
        sprintf(suca, "ROOM %2x", selection);
        DrawText(suca, 20, 20, 30, PURPLE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

