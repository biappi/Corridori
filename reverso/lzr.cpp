#include "common.h"
#include "lzr.h"

void save_arc_lzr() {}
void load_arc_lzr(pstring_t param1) {}
void close_arc_lzr() {}

void set_arc_lzr_handling_funcs(void (*save)(), void (*load)(pstring_t), void (*close)()) {
  save_arc_lzr_ptr = save_arc_lzr;
  load_arc_lzr_ptr = load_arc_lzr;
  close_arc_lzr_ptr = close_arc_lzr;
}
