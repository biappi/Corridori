typedef uint8_t set_t[32];

set_t set_1;
set_t set_2;


// di, cs, ax
// Set is a 256 bit bitset, this function checks if the i-th bit is set.
Set::MemberOf(uint8_t index, set_t set) {
  return (1 << (index % 8)) & set[index / 8];
}

void choose_left_right_tile(uint8_t facing_right, uint8_t &pupo_x) {
  // Stack:
  // sp+0   oldbp  <- bp
  // sp+1   oldbp
  // sp+2   ip
  // sp+3   ip
  // sp+6   cs
  // sp+7   cs
  // sp+8   oldbp
  // sp+9   oldbp
  // sp+10  ...    <- oldbp

  // push    bp
  // mov     bp, sp
  // mov     di, [bp+6]      ss:[bp+6] -> oldbp
  // cmp     ss:[di-3], 0    ss:[di-3] = ss:[oldbp-3] -> facing_right
  if (facing_right == 0) pupo_x = 3;
  else pupo_x = 56;
}

// stack: es, di (current_ani_from_framestab), ax(pupo_tile_top), cs, ip, bp

uint16_t logitab_segment, logitab_offset;
uint16_t sostani_segment, sostani_offset;

dammi_nuovo_current_ani(uint8_t pupo_tile_top, uint8_t &current_ani_from_framestab) {
  uint16_t loc_logitab_segment; // bp-1, bp-2
  uint16_t loc_logitab_offset; // bp-3, bp-4
  uint16_t loc_sostani_segment; // bp-5, bp-6
  uint16_t loc_sostani_offset; // bp-7, bp-8
  uint8_t local_current_ani_from_framestab; // bp-9
  uint8_t unk4; // bp-0A

  uint16_t s_idx;
  uint16_t l_idx;
  uint8_t *logitab;
  uint8_t *sostani;

  loc_logitab_offset = logitab_offset;
  loc_logitab_segment = logitab_segment;
  loc_sostani_offset = sostani_offset;
  loc_sostani_segment = sostani_segment;

  logitab = loc_logitab_segment * 16 + loc_logitab_offset;
  sostani = loc_sostani_segment * 16 + loc_sostani_offset;

  s_idx = sostani[0] * 256 + sostani[1]

  tag1:

  while (sostani[s_idx] != 0xff && sostani[s_idx] != current_ani_from_framestab)
    s_idx += 3;

  if (sostani[s_idx] == current_ani_from_framestab) {
    l_idx = logitab[sostani[s_idx+1] * 2] * 256 + logitab[sostani[s_idx+1] * 2 + 1];

    while (logitab[l_idx] != 0xff && logitab[l_idx] != pupo_tile_top)
      l_idx++;

    if (logitab[l_idx] == pupo_tile_top) {
      current_ani_from_framestab = sostani[s_idx+2];
    } else {
      s_idx += 3;
      goto tag1;
    }
  }
}

uint8_t gun_bool;

struct gun_stuff_t {
  uint8_t gun_ref1[2];
  uint8_t unk1[2];
  uint16_t gun_ref2[2];
} gun_stuff;

uint8_t gun_ref_save1;
uint16_t gun_ref_save2;

void update_gun_ref_save() {
  gun_ref_save1 = gun_stuff.gun_ref1[gun_bool];
  gun_ref_save2 = gun_stuff.gun_ref2[gun_bool];
}

void set_ani_67_32(uint8_t facing_right, uint8_t &current_ani_from_framestab) {
  if (facing_right) current_ani_from_framestab = 0x67;
  else current_ani_from_framestab = 0x32;
}

struct logitab_t {
  uint16_t header_size;
  uint16_t *entries;
  // Sequence i begins at index entries[i]-header_size
  uint8_t *sequences;
} logitab;

enum logitab_enum_t {
  LOGITAB_ENTRY_00, LOGITAB_ENTRY_01, LOGITAB_ENTRY_02, LOGITAB_ENTRY_03,
  LOGITAB_ENTRY_04, LOGITAB_ENTRY_05, LOGITAB_ENTRY_06, LOGITAB_ENTRY_07,
  LOGITAB_ENTRY_08, LOGITAB_ENTRY_09, LOGITAB_ENTRY_0A, LOGITAB_ENTRY_0B,
  LOGITAB_ENTRY_0C, LOGITAB_ENTRY_0D, LOGITAB_ENTRY_0E, LOGITAB_ENTRY_0F,
  LOGITAB_ENTRY_10, LOGITAB_ENTRY_11, LOGITAB_ENTRY_12, LOGITAB_ENTRY_13,
  LOGITAB_ENTRY_14, LOGITAB_ENTRY_15, LOGITAB_ENTRY_16, LOGITAB_ENTRY_17,
  LOGITAB_ENTRY_18, LOGITAB_ENTRY_19, LOGITAB_ENTRY_1A, LOGITAB_ENTRY_1B,
  LOGITAB_ENTRY_1C, LOGITAB_ENTRY_1D, LOGITAB_ENTRY_1E, LOGITAB_ENTRY_1F,
  LOGITAB_ENTRY_20, LOGITAB_ENTRY_21, LOGITAB_ENTRY_22, LOGITAB_ENTRY_23,
  LOGITAB_ENTRY_24, LOGITAB_ENTRY_25, LOGITAB_ENTRY_26, LOGITAB_ENTRY_27,
  LOGITAB_ENTRY_28, LOGITAB_ENTRY_29, LOGITAB_ENTRY_2A, LOGITAB_ENTRY_2B,
  LOGITAB_ENTRY_2C, LOGITAB_ENTRY_2D, LOGITAB_ENTRY_2E, LOGITAB_ENTRY_2F
}

bool check_if_thing_is_present_in_logi_tab(uint8_t value_to_find, logitab_enum_t entry_idx) {
  uint8_t *seq_ptr = logitab.sequences[logitab.entries[entry_idx] - logitab.header_size];
  while (*seq_ptr != 0xff) {
    if (*(seq_ptr++) == value_to_find) return true;
  }
}

void controlla_che_cho_sotto_i_piedi(
  uint8_t &current_ani_from_framestab,
  uint8_t pupo_tile_top,
  uint8_t pupo_tile_bottom,
  uint16_t pupo_x,
  uint16_t pupo_y,
  uint8_t saved_pupo_x,
  uint8_t saved_pupo_y) {

  // facing_right= byte ptr -3
  // var_2= byte ptr -2
  // wanted_room= byte ptr -1
  // saved_pupo_y= byte ptr  6
  // saved_pupo_x= byte ptr  8
  // pupo_y= word ptr  0Ah
  // pupo_x= word ptr  0Ch
  // pupo_tile_bottom= byte ptr  0Eh
  // pupo_tile_top= byte ptr  10h
  // current_ani_from_framestab= dword ptr  12h

  char wanted_room;
  char var_2;
  char facing_right;
  char sonasega;

  var_2 = 0;

  facing_right = 0;
  if (current_ani_from_framestab >= 0x35) facing_right = 1;

  if (*(char *)(0x956) != 0 || pupo_tile_top != 0xfc) {
    if (check_if_thing_is_present_in_logi_tab(pupo_tile_bottom, LOGITAB_ENTRY_25) == 0) {
      if (*(char *)(0x956) <= 0) {
        if (pupo_tile_top != 9) {
          if (pupo_tile_top != 0) {
            if (*(char *)(0x957) <= 0) {
              if (*(char *)(0x958) >= 0) {
                *(char *)(es:0x958)--;
                if (Set::MemberOf(current_ani_from_framestab, set_1)) {
                  choose_left_right_tile(facing_right, pupo_x);
                  var_2 = 1;
                }
              }
              if (var_2 == 0) {
                if (*(char*)(0x959) >= 0) {
                  *(char *)(es:0x959)--;
                  es:di <- &current_ani_from_framestab;
                  al <- current_ani_from_framestab;
                  if (Set::MemberOf(current_ani_from_framestab, set_2)) {
                    choose_left_right_tile(facing_right, pupo_x);
                    var_2 = 1;
                  }
                }
              }
              if (var_2 == 0)
                dammi_nuovo_current_ani(pupo_tile_top, current_ani_from_framestab);
              // Colpishi
              if (current_ani_from_framestab == 0x12 || current_ani_from_framestab == 0x13) {
                // sub_11A14
                if (gun_bool == 0 && colpi == 0) {
                  pupo_x = 3;
                  gun_bool = 1;
                  update_gun_ref_save();
                }
              }
              else if (current_ani_from_framestab == 0x47 || current_ani_from_framestab == 0x48) {
                // sub_11A3D
                if (gun_bool == 0 && colpi == 0) {
                  pupo_x = 0x38;
                  gun_bool = 1;
                  update_gun_ref_save();
                }
              }
              else if (current_ani_from_framestab == 0x34) {
                // call to an empty function
              }
              else if (current_ani_from_framestab == 0x69) {
                // call to an empty function
              }

              // stack: ax cs di cs ip
              if (Set::MemberOf(current_ani_from_framestab, set_1)) {
                *(char *)(0x0958) = 4;
              }
              else if (Set::MemberOf(current_ani_from_framestab, set_2)) {
                *(char *)(0x0959) = 4;
              }
              else if (current_ani_from_framestab == 0x14) {
                // sub_11A74
                // stack 0x00 0xd0 bp cs ip bp
                uint16_t var1 = saved_pupo_x + (int16_t)((int8_t)(0xd0));
                if (var1 >= 0 && var1 <= 0x1ef) {
                  get_tile_type_for_x_y(var1, saved_pupo_y, &wanted_room);
                  if (wanted_room == 0) {
                    current_ani_from_framestab = 0x00;
                  }
                }
              }
              else if (current_ani_from_framestab == 0x49) {
                // sub_11A74
                // stack 0x35 0x30 bp cs ip bp
                uint16_t var1 = saved_pupo_x + (int16_t)((int8_t)(0x30));
                if (var1 >= 0 && var1 <= 0x1ef) {
                  get_tile_type_for_x_y(var1, saved_pupo_y, &wanted_room);
                  if (wanted_room == 0) {
                    current_ani_from_framestab = 0x35;
                  }
                }
              }
              else if (current_ani_from_framestab == 0x08) {
                // sub_11A74
                // stack 0x15 0xd0 bp cs ip bp
                uint16_t var1 = saved_pupo_x + (int16_t)((int8_t)(0xd0));
                if (var1 >= 0 && var1 <= 0x1ef) {
                  get_tile_type_for_x_y(var1, saved_pupo_y, &wanted_room);
                  if (wanted_room == 0) {
                    current_ani_from_framestab = 0x15;
                  }
                }
              }
              else if (current_ani_from_framestab == 0x09) {
                // sub_11A74
                // stack 0x16 0xd0 bp cs ip bp
                uint16_t var1 = saved_pupo_x + (int16_t)((int8_t)(0xd0));
                if (var1 >= 0 && var1 <= 0x1ef) {
                  get_tile_type_for_x_y(var1, saved_pupo_y, &wanted_room);
                  if (wanted_room == 0) {
                    current_ani_from_framestab = 0x16;
                  }
                }
              }
              else if (current_ani_from_framestab == 0x3D) {
                // sub_11A74
                // stack 0x4a 0x30 bp cs ip bp
                uint16_t var1 = saved_pupo_x + (int16_t)((int8_t)(0x30));
                if (var1 >= 0 && var1 <= 0x1ef) {
                  get_tile_type_for_x_y(var1, saved_pupo_y, &wanted_room);
                  if (wanted_room == 0) {
                    current_ani_from_framestab = 0x4a;
                  }
                }
              }
              else if (current_ani_from_framestab == 0x3E) {
                // sub_11A74
                // stack 0x4b 0x30 bp cs ip bp
                uint16_t var1 = saved_pupo_x + (int16_t)((int8_t)(0x30));
                if (var1 >= 0 && var1 <= 0x1ef) {
                  get_tile_type_for_x_y(var1, saved_pupo_y, &wanted_room);
                  if (wanted_room == 0) {
                    current_ani_from_framestab = 0x4b;
                  }
                }
              }
              else if (current_ani_from_framestab == 0x10) {
                // sub_11AC6
                // stack bp, cs, ip, bp
                uint8_t var1 = 0;
                if (check_if_thing_is_present_in_logi_tab(pupo_tile_top, LOGITAB_ENTRY_26)) {
                  if (saved_pupo_x - 0x10 >= 0) {
                    get_tile_type_for_x_y(saved_pupo_x - 0x10, saved_pupo_y, &wanted_room);
                    if (wanted_room == pupo_tile_top) {
                      current_ani_from_framestab = 0x1D;
                      var1 = 1;
                    }
                  }
                }
                if (var1 == 0) {
                  if (check_if_thing_is_present_in_logi_tab(pupo_tile_top, LOGITAB_ENTRY_28)) {
                    if (saved_pupo_x - 0x10 >= 0) {
                      get_tile_type_for_x_y(saved_pupo_x - 0x10, saved_pupo_y, &wanted_room);
                      if (wanted_room == pupo_tile_top) {
                        current_ani_from_framestab = 0x1D;
                      }
                    }
                  }
                }
              }
              else if (current_ani_from_framestab == 0x45) {
                // sub_11B94
                // stack bp, cs, ip, bp
                uint8_t var1 = 0;
                if (check_if_thing_is_present_in_logi_tab(pupo_tile_top, LOGITAB_ENTRY_26)) {
                  if (saved_pupo_x + 0x10 <= 0x13F) {
                    get_tile_type_for_x_y(saved_pupo_x + 0x10, saved_pupo_y, &wanted_room);
                    if (wanted_room == pupo_tile_top) {
                      current_ani_from_framestab = 0x52;
                      var1 = 1;
                    }
                  }
                }
                if (var1 == 0) {
                  if (check_if_thing_is_present_in_logi_tab(pupo_tile_top, LOGITAB_ENTRY_28)) {
                    if (saved_pupo_x + 0x10 < 0x13F) {
                      get_tile_type_for_x_y(saved_pupo_x + 0x10, saved_pupo_y, &wanted_room);
                      if (wanted_room == pupo_tile_top) {
                        current_ani_from_framestab = 0x52;
                      }
                    }
                  }
                }

              }
              else if (current_ani_from_framestab == 0x1F || current_ani_from_framestab == 0x20 || current_ani_from_framestab == 0x54current_ani_from_framestab == 0x54) {
                eventually_change_room();
              }
              else if (current_ani_from_framestab == 0x21 || current_ani_from_framestab == 0x55) {
                check_tile_and_switch_room();
              }
            }
            else if (*(char *)(0x957) > 8){
              if (facing_right == 0) current_ani_from_framestab = 0x33;
              else current_ani_from_framestab = 0x68;
              *(char *)(0x957) = 0;
            }
            else *(char *)(0x957) = 0;
          }
          else set_ani_67_32(facing_right, current_ani_from_framestab);
        }
        else {
          *(char *)(0x956) = 8;
          pupo_bob_per_bg_arg0_palette_override = pupo_y + 0x0a - pupo_palette_delta;
          if (*(char*)(0x972) != 0) empty_func();
          set_ani_67_32(facing_right, current_ani_from_framestab);
        }
      }
      else {
        *(char *)(0x956)--;
        if (*(char *)(0x956) == 0) {
          if (*(char *)(0x972) == 0) set_ani_67_32(facing_right, current_ani_from_framestab);
          else sub_11711(); // TODO
        }
      }
    }
    else set_ani_67_32(facing_right, current_ani_from_framestab);
  }
  else {
    get_tile_type_for_x_y(pupo_x, pupo_y - 0x0a, &wanted_room);
    if (wanted_room != 0) {
      pupo_bob_per_bg_arg0_palette_override = 0xc7 - pupo_palette_delta;
      if (*(char *)0x0972 != 0) empty_func();
    }
    else {
      if (*(char *)0x0972 != 0) empty_func();
    }
    set_ani_67_32(facing_right, current_ani_from_framestab);
  }
}
