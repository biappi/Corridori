#include "video.h"

void init_video_area(int16_t min_col, int16_t min_row, int16_t max_col, int16_t max_row) {
  if (min_col < 0) min_col = 0;
  if (min_col > 319) min_col = 319;

  if (max_col < 0) max_col = 0;
  if (max_col > 319) max_col = 319;

  if (min_col > max_col) {
    min_col = 0;
    max_col = 319;
  }

  video_min_col = min_col;
  video_max_col = max_col;

  if (min_row < 0) min_row = 0;
  if (min_row > 199) min_row = 199;

  if (max_row < 0) max_row = 0;
  if (max_row > 199) max_row = 199;

  if (min_row > max_row) {
    min_row = 0;
    max_row = 199;
  }

  video_min_row = min_row;
  video_max_row = max_row;

  video_row_sz = max_row - min_row + 1;
  video_col_sz = max_col - min_col + 1;

  video_top_left_abs_idx = min_col + min_row * 320;

  if (video_col_sz == 320) {
    video_half_screen_abs_idx = video_row_sz * 160;
  }
}
