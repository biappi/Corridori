#ifndef CORR_KEYBOARD_STATE_H
#define CORR_KEYBOARD_STATE_H

typedef unsigned char uint8_t;

#include "common.h"
#include "placeholders.h"

typedef struct keyboard_state_struct {
  uint8_t up_pressed;
  uint8_t down_pressed;
  uint8_t left_pressed;
  uint8_t right_pressed;
  uint8_t home_pressed;
  uint8_t end_pressed;
  uint8_t pgup_pressed;
  uint8_t pgdown_pressed;
  uint8_t pressed_unk_1;
  uint8_t pressed_unk_2;
  uint8_t pressed_unk_3;
  uint8_t l_pressed;
  uint8_t s_pressed;
  uint8_t n_pressed;
  uint8_t m_o_comma_pressed;
  uint8_t i_pressed;
  uint8_t esc_pressed;
  uint8_t p_pressed;
  uint8_t j_pressed;
  uint8_t h_pressed;
  uint8_t u_pressed;
  uint8_t l_shift_pressed;
  uint8_t r_shift_pressed;
  uint8_t enter_pressed;
  uint8_t pressed_unk_4;
  uint8_t pressed_unk_5;
  uint8_t pressed_unk_6;
  uint8_t pressed_unk_7;
  uint8_t pressed_unk_8;
} keyboard_state_t;

static keyboard_state_t keyboard_state;
static void (*old_keyboard_ISR)();
static uint8_t byte_22E32;

void reset_keyboard();
void get_old_keyboard_ISR();
void install_keyboard_ISR();
void keyboard_ISR();

uint8_t read_from_joystick_ISR();

uint8_t is_keyboard_or_joystick();

static uint16_t joystick_setting_1;
static uint16_t joystick_setting_2;
static uint16_t joystick_setting_3;
static uint16_t joystick_setting_4;

void sub_16540();
void joystick_calibration();

static uint8_t is_old_keyboard_ISR;
static void* read_from_keyboard_ISR;

#endif
