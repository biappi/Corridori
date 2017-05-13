#ifndef CORR_PLAYER_H
#define CORR_PLAYER_H

static uint8_t (*player_keyboard_update_handler)();

static uint8_t byte_21602;
static uint8_t byte_1F528;
static uint8_t byte_21606;
static uint8_t byte_21603;

static uint8_t byte_1F489;

void init_player_keyboard_update_handler();
uint8_t player_keyboard_update();
uint8_t player_joystick_update();
void sub_13580();
uint8_t sub_11051();

#endif
