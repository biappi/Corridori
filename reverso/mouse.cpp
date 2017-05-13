#include "mouse.h"
#include "error.h"

void init_mouse() {
  mouse_initialized = 0;
  if (settings[1] != 0x01) return;
  if (reset_mouse() == 0) {
    halt_with_error(mouse_empty_string, 0x17, mouse_init_error);
  }
  mouse_initialized = 1;
  set_mouse_range(0, 0, 319, 199);
  position_mouse(160, 100);
}

uint8_t reset_mouse() {

}

void set_mouse_range(uint16_t min_col, uint16_t min_row, uint16_t max_col, uint16_t max_row) {
  if (max_col > 319) max_col = 319;
  if (max_row > 199) max_row = 199;
  asm {
    mov ax, 7;
    mov cx, min_col;
    mov dx, max_col;
    int 0x33;

    mov ax, 8;
    mov cx, min_row;
    mov dx, max_row;
    int 0x33;
  }
}

void position_mouse(uint16_t col, uint16_t row) {
  asm {
    mov ax, 4;
    mov cx, col;
    mov dx, row;
    int 0x33;
  }
}
