#include "keyboard.h"

void reset_keyboard() {
  keyboard_state.up_pressed = 0;
  keyboard_state.down_pressed = 0;
  keyboard_state.left_pressed = 0;
  keyboard_state.right_pressed = 0;
  keyboard_state.home_pressed = 0;
  keyboard_state.end_pressed = 0;
  keyboard_state.pgup_pressed = 0;
  keyboard_state.pgdown_pressed = 0;
  keyboard_state.pressed_unk_1 = 0;
  keyboard_state.pressed_unk_2 = 0;
  keyboard_state.pressed_unk_3 = 0;
  keyboard_state.l_pressed = 0;
  keyboard_state.s_pressed = 0;
  keyboard_state.n_pressed = 0;
  keyboard_state.m_o_comma_pressed = 0;
  keyboard_state.i_pressed = 0;
  keyboard_state.esc_pressed = 0;
  keyboard_state.p_pressed = 0;
  keyboard_state.j_pressed = 0;
  keyboard_state.h_pressed = 0;
  keyboard_state.u_pressed = 0;
  keyboard_state.l_shift_pressed = 0;
  keyboard_state.r_shift_pressed = 0;
  keyboard_state.enter_pressed = 0;
  keyboard_state.pressed_unk_4 = 1;
  keyboard_state.pressed_unk_5 = 1;
  keyboard_state.pressed_unk_6 = 1;
  keyboard_state.pressed_unk_7 = 1;
  keyboard_state.pressed_unk_8 = 0;
}

void get_old_keyboard_ISR() {
  // Gets the function pointer of the n-th ISR from the ISR vector
  GetIntVec(0x09, old_keyboard_ISR);
  byte_22E32 = 0;
  is_old_keyboard_ISR = 1;
  reset_keyboard();
}

void install_keyboard_ISR() {
  SetIntVec(0x09, keyboard_ISR);
  is_old_keyboard_ISR = 0;
  reset_keyboard();
}

void keyboard_ISR() {

}
