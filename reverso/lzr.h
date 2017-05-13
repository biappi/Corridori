#ifndef CORR_LZR_H
#define CORR_LZR_H

#include "common.h"
#include "pascal/string.h"

static void(*save_arc_lzr_ptr)();
static void(*load_arc_lzr_ptr)(pstring_t);
static void(*close_arc_lzr_ptr)();

void save_arc_lzr();
void load_arc_lzr(pstring_t param1);
void close_arc_lzr();

void set_arc_lzr_handling_funcs(void (*save)(), void (*load)(pstring_t), void (*close)());

#endif
