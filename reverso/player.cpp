#include "keyboard.h"
#include "player.h"
#include "common.h"

void init_player_keyboard_handler() {
  if (is_keyboard_or_joystick() == 0)
    player_keyboard_update_handler = player_keyboard_update;
  else {
    sub_16540();
    player_keyboard_update_handler = player_joystick_update;
  }
}

void sub_13580() {
  byte_21602 = 0;
  byte_1F528 = 0;
  byte_21606 = 0;
  byte_21603 = 0;
}

uint8_t player_keyboard_update() {
  uint8_t ret = 0;
  int8_t delta_x;
  int8_t delta_y;

  if (keyboard_state.up_pressed) delta_y--;
  if (keyboard_state.down_pressed) delta_y++;
  if (keyboard_state.left_pressed) delta_x--;
  if (keyboard_state.right_pressed) delta_x++;
  if (keyboard_state.home_pressed) {
    delta_x--;
    delta_y--;
  }
  if (keyboard_state.end_pressed) {
    delta_x--;
    delta_y++;
  }
  if (keyboard_state.pgup_pressed) {
    delta_x++;
    delta_y--;
  }
  if (keyboard_state.pgdown_pressed) {
    delta_x++;
    delta_y++;
  }

  if (delta_y < 0) delta_y = -1;
  if (delta_y > 0) delta_y = 1;

  // Move left
  if (delta_x < 0) {
    // ret {5}
    if (delta_y > 0) ret = 5;
    // ret {0, 1}
    else ret = -delta_y;
  }
  // Move right
  else if (delta_x > 0) {
    // ret {2, 3, 4}
    ret = 3 + delta_y;
  }
  // delta_x == 0, move only vertical
  else {
    // ret {6, 7, 8}
    ret = 7 - delta_y;
  }

  if (keyboard_state.l_shift_pressed || keyboard_state.r_shift_pressed) {
    // ret {9}
    ret = 9;
  }

  if (ret != 0) sub_13580();

  return ret;
}

// TODO: recheck
uint8_t sub_1AE2D(uint8_t param1, uint8_t param2) {
  uint8_t *roba = (uint8_t*)(0x7DB + param1 + 2 * param2);
  uint8_t var_2 = *(roba);
  uint8_t joystick_data;
  asm {
    mov dx, 0x201;
    in al, dx;
    mov joystick_data, al;
  }
  if ((joystick_data & var_2) != var_2) joystick_data++;

  return joystick_data;
}

uint8_t sub_11051() {
  uint8_t ret = read_from_joystick_ISR();
  if (sub_1AE2D(byte_1F489, 1) || sub_1AE2D(byte_1F489, 2)) {
    ret += 9;
  }
  return ret;
}

uint8_t player_joystick_update() {
  uint8_t var_2 = sub_11051();
  uint8_t var_3 = player_keyboard_update();

  if (var_3 <= 0) return var_2;
  if (var_3 == 9 && var_2 <= 9) var_2 += 9;
  if (var_3 == 9 || var_2 >= 9) {
    if (var_2 >= 9 && var_3 < 9) {
      var_3 += 9;
      var_2 = var_3;
    }
    else {
      var_2 = var_3;
    }
  }
  return var_2;
}
