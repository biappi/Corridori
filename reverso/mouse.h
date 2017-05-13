#ifndef CORR_MOUSE_H
#define CORR_MOUSE_H

#include "common.h"
#include "pascal/string.h"
#include "linkage.h"

static uint8_t mouse_initialized;
static const pstring_t mouse_empty_string;

void init_mouse();
uint8_t reset_mouse();
void set_mouse_range(uint16_t min_col, uint16_t min_row, uint16_t max_col, uint16_t max_row);
void position_mouse(uint16_t col, uint16_t row);

#endif
