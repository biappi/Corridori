#ifndef CORR_VIDEO_H
#define CORR_VIDEO_H

#include "common.h"

static uint16_t video_min_col;
static uint16_t video_max_col;
static uint16_t video_min_row;
static uint16_t video_max_row;

static uint16_t video_col_sz;
static uint16_t video_row_sz;
static uint16_t video_top_left_abs_idx;
static uint16_t video_half_screen_abs_idx;

static uint16_t background_buffer;

void init_video_area(int16_t min_col, int16_t min_row, int16_t max_col, int16_t max_row);
#endif
