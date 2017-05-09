#include <algorithm>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "error.h"
#include "keyboard_state.h"
#include "placeholders.h"
#include "pascal/string.h"

uint8_t some_global_1;
uint8_t some_global_2;
uint8_t some_global_3;
uint8_t some_global_4;
uint8_t some_global_5;

void (*old_keyboard_ISR)();

uint8_t byte_22E32;
uint8_t is_old_keyboard_ISR;
uint16_t read_from_keyboard_ISR[2];

uint8_t byte_22CDC;
uint8_t byte_22CDE;
uint8_t esc_pressed_flag;
uint8_t byte_22CE0;
uint16_t loaded_font_buffer_size;
char loaded_font_filename[0x100];

uint8_t byte_22E1C;
uint8_t byte_22E1D;
uint8_t byte_22E1E;
uint8_t byte_22E1F;

uint16_t word_228AA;

void reset_some_globals() {
  some_global_1 = 0;
  some_global_2 = 0;
  some_global_3 = 0;
  some_global_4 = 0;
  some_global_5 = 255;
}

void get_old_keyboard_ISR() {
  // Gets the function pointer of the n-th ISR from the ISR vector
  GetIntVec(0x09, old_keyboard_ISR);
  byte_22E32 = 0;
  is_old_keyboard_ISR = 1;
  reset_keyboard();
}

void reset_some_globals_2() {
  byte_22E1C = 0;
  byte_22E1D = 0;
  byte_22E1E = 0;
  byte_22E1F = 0;
}

void reset_some_globals_3() {
  word_228AA = 0;
}

void sub_1B60C() {
  byte_22CDC = 0;
  loaded_font_filename[0] = 0;
  read_from_keyboard_ISR[0] = read_from_keyboard_ISR[1] = 0;
  byte_22CDE = 0;
  esc_pressed_flag = 0;
  byte_22CE0 = 0;
  reset_some_globals_2();
}

uint16_t word_22798;
uint16_t room_irm_size;
uint16_t buffer_mat_content[2];
uint8_t byte_227A6;
uint16_t unknown_array_1[50];
uint8_t nr_bobs_per_background;

void initialize_room_irm_and_buffer_mat() {
  word_22798 = 0;
  room_irm_size = 0;
  buffer_mat_content[0] = 0;
  buffer_mat_content[1] = 0;
  byte_227A6 = 1;

  do {
    unknown_array_1[byte_227A6-1] = 0;
    byte_227A6++;
  } while (byte_227A6 != 50);
  nr_bobs_per_background = 0;
}

uint16_t numeri_ele_filesize_minus_header;
uint16_t status_ele_filesize_minus_header;
uint16_t word_21AC8;

void sub_195F8() {
  numeri_ele_filesize_minus_header = 0;
  status_ele_filesize_minus_header = 0;
  word_21AC8 = 0;
}

uint16_t pu_file_size;
uint16_t ucci_ele_size;
uint16_t ucci_tab_file_size;
uint16_t word_21966;
uint16_t word_21968;
uint16_t ucci_ele_content;
uint16_t word_21964;
uint16_t ucci_image_content[2];

void init_ucci_pu() {
  pu_file_size = 0;
  ucci_ele_size = 0;
  ucci_tab_file_size = 0;
  word_21966 = 0;
  word_21968 = 0;
  ucci_ele_content = 0;
  word_21964 = 0;
  ucci_image_content[0] = 0;
  ucci_image_content[1] = 0;
}

uint16_t prev_button_status;

void init_mouse_click() {
  prev_button_status = 0;
}

const char *aGame_dir = "GAME_DIR";
const char *aSimulmondo = "SIMULMONDO";
const char *empty_string = "";
char* game_dir_path;

void init_game_dir_path() {
  // memcpy(game_dir_path, aGame_dir, std::min(sizeof(aGame_dir), sizeof(game_dir_path)));
  PStringOperatorEquals(aGame_dir, game_dir_path, 0x45);
}

uint8_t byte_22CDD;
uint16_t swi_filesize;
uint16_t gsa_filesize;
uint16_t ptx_filesize;
void(*func_ptr_1)();
void(*func_ptr_2)();
void(*func_ptr_3)();
void(*func_ptr_4)();
void(*func_ptr_5)();
void(*func_ptr_6)();
void(*func_ptr_7)();
void(*func_ptr_8)();

void sub_146F0() {}
void sub_147AB() {}
void sub_1488D() {}
void sub_1493F() {}
void sub_14A12() {}
void sub_14ACD() {}
void sub_14BF5() {}
void cambia_tile_dopo_caricamento(uint8_t) {}

void init_function_pointers() {
  byte_22CDD = 0;
  swi_filesize = 0;
  gsa_filesize = 0;
  ptx_filesize = 0;
  func_ptr_1 = sub_146F0;
  func_ptr_2 = sub_147AB;
  func_ptr_3 = sub_1488D;
  func_ptr_4 = sub_1493F;
  func_ptr_5 = sub_14A12;
  func_ptr_6 = sub_14ACD;
  func_ptr_7 = sub_14BF5;
  func_ptr_8 = (void (*)()) cambia_tile_dopo_caricamento;
}

uint16_t pn_file_size;
uint16_t ele_filename;

void sub_146D8() {
  pn_file_size = 0;
  ele_filename = 0;
}

uint8_t byte_21602;
uint8_t byte_1F528;
uint8_t byte_21606;

void sub_144D7() {
  byte_21602 = 0;
  byte_1F528 = 0;
  byte_21606 = 0;
}

uint16_t mouse_x;
uint16_t mouse_y;
uint16_t mouse_pointer_file_size;

void init_mouse_pointer() {
  mouse_x = 0xA8;
  mouse_y = 0x64;
  mouse_pointer_file_size = 0;
}

uint16_t animjoy_tab_file_size;
uint16_t frames_tab_file_size;
uint16_t animofs_tab_file_size;
uint16_t logi_tab_file_size;
uint16_t sostani_file_size;
uint16_t prt_file_size;
uint16_t usc_file_size;
uint16_t dsp_file_size;
uint16_t tr_ele_file_size;
uint16_t tr_ele_file_size_2;
uint8_t byte_1F502;
void(*capisci_dove_muovere_il_pupo_ptr)();
void(*current_funcptr_1)();
void(*current_funcptr_2)();

void capisci_dove_muovere_il_pupo_1() {}
void funcptr_1() {}
void funcptr_2() {}

void init_pupo() {
  animjoy_tab_file_size = 0;
  frames_tab_file_size = 0;
  animofs_tab_file_size = 0;
  logi_tab_file_size = 0;
  sostani_file_size = 0;
  prt_file_size = 0;
  usc_file_size = 0;
  dsp_file_size = 0;
  tr_ele_file_size = 0;
  tr_ele_file_size_2 = 0;
  byte_1F502 = 0;
  capisci_dove_muovere_il_pupo_ptr = capisci_dove_muovere_il_pupo_1;
  current_funcptr_1 = funcptr_1;
  current_funcptr_2 = funcptr_2;
}

void(*save_arc_lzr_ptr)();
void(*load_arc_lzr_ptr)();
void(*close_arc_lzr_ptr)();

void save_arc_lzr() {}
void load_arc_lzr() {}
void close_arc_lzr() {}

void set_arc_lzr_handling_funcs(void (*save)(), void (*load)(), void (*close)()) {
  save_arc_lzr_ptr = save_arc_lzr;
  load_arc_lzr_ptr = load_arc_lzr;
  close_arc_lzr_ptr = close_arc_lzr;
}

uint8_t byte_21858;
uint8_t disable_pupo_anim_decrement;

void(*atexit_ptr)();
void(*deinit_1)();
void(*deinit_2)();

void send_things_back_to_manager() {}
void reset_keyboard_and_sound_and_boh() {}

void deinit(void(*reset_keyboard_and_sound_and_boh)(), void(*send_things_back_to_manager)()) {
  deinit_1 = send_things_back_to_manager;
  deinit_2 = reset_keyboard_and_sound_and_boh;
}

void sub_16B60(void (*atexit)(), void(*reset_keyboard_and_sound_and_boh)()) {
  atexit_ptr = atexit_ptr;
  deinit(reset_keyboard_and_sound_and_boh, &send_things_back_to_manager);
}

// Converts a 4 character hex string to a value
uint16_t pointer_string_to_value(char *string) {
  // char local_var1[4];
  char tmp;
  uint16_t ret = 0;
  // PStringOperatorEquals(string, local_var1, 4);

  for (int i = 0; i < 4; i++) {
    tmp = string[i];
    if (tmp >= '0' && tmp <= '9') {
      ret += (tmp - '0') << (4 * i);
    }
    else if (tmp >= 'a' && tmp <= 'f') {
      ret += (tmp - 'a' + 10) << (4 * i);
    }
    else if (tmp >= 'A' && tmp <= 'F') {
      ret += (tmp - 'A' + 10) << (4 * i);
    }
  }

  return ret;
}



void halt_with_error(const char* message, int retval, int error_string_id) {
  char* string_local;
  char* final_error_msg;

  PStringOperatorEquals(message, string_local, 0x45);
  if (error_string_id == 0) PStringOperatorEquals(string_local, byte_22A1C, 0x45);
  else {
    PStringOperatorEquals(final_error_msg, error_strings[error_string_id-1]);
    PStringConcat(final_error_msg, string_local);
    PStringOperatorEquals(final_error_msg, byte_22A1C, 0x45);
  }
  byte_22CDE = 1;
  deinit_1();
  deinit_2();
  Halt(retval);
}

typedef struct {
  char header[11];
  char interface_version_string[4];
  char language[3];
} __attribute__((packed)) linkage_area_t;

uint16_t linkage_area_in_manager_seg, linkage_area_in_manager_off;
uint8_t parse_linkage_area(int argc, char** argv) {
  uint8_t ret = 0;
  const char *sColon = ":";
  char *temp = NULL;
  char *linkage_point_as_string = NULL;
  uint16_t link_segment_from_str, link_offset_from_str;
  uint16_t link_seg, link_off;
  uint32_t link;
  uint8_t idx;
  uint8_t interface_version_string;
  char header[10];
  linkage_area_t linkage_header;

  if (argc == 2) {
    temp = (char*)malloc((strlen(argv[1]) + 1) * sizeof(char));
    memset(temp, 0, (strlen(argv[1]) + 1) * sizeof(char));
    strcpy(temp, argv[1]);
    // Source, destination, truncate
    PStringOperatorEquals(temp, linkage_point_as_string, 0xFF);
    if (strlen(linkage_point_as_string) != 9) return ret;
    // substr, str
    if (PStringPos(sColon, linkage_point_as_string) != 5) return ret;
    // destination, source, index, count
    PStringCopy(temp, linkage_point_as_string, 1, 4);
    memcpy(temp, linkage_point_as_string, 4 * sizeof(char));
    link_segment_from_str = pointer_string_to_value(temp);
    PStringCopy(temp, linkage_point_as_string, 6, 4);
    link_offset_from_str = pointer_string_to_value(temp);
    linkage_area_in_manager_seg = link_segment_from_str;
    linkage_area_in_manager_off = link_offset_from_str;
    link_seg = linkage_area_in_manager_seg;
    link_off = linkage_area_in_manager_off;
    link = (link_seg << 16) + link_off;
    // header = 0x0A;


    idx = 0x00;

    memcpy(&linkage_header, (void*)link, sizeof(linkage_area_t));
    printf("%s\n", (char*)((&linkage_header.header) + 1));
    printf("%s\n", (char*)((&linkage_header.interface_version_string) + 1));
    printf("%s\n", (char*)((&linkage_header.language) + 1));

    // This is a check for MANAGER <-> ARCADE correct linkage
    // It won't work of course
    if (!PStringOperatorMinus(header, aSimulmondo)) {
      halt_with_error(empty_string, 1, interface_header_error);
    }

    interface_version_string = 3;
  }
  return ret;
}

int main(int argc, char** argv) {
  reset_some_globals();
  get_old_keyboard_ISR();
  sub_1B60C();
  reset_some_globals_3();
  initialize_room_irm_and_buffer_mat();
  sub_195F8();
  init_ucci_pu();
  init_mouse_click();
  init_game_dir_path();
  init_function_pointers();
  sub_146D8();
  sub_144D7();
  init_mouse_pointer();
  init_pupo();
  byte_21858 = 0;
  byte_22CDC = 0;
  disable_pupo_anim_decrement = 0;
  set_arc_lzr_handling_funcs(save_arc_lzr, load_arc_lzr, close_arc_lzr);
  sub_16B60(atexit, reset_keyboard_and_sound_and_boh);
  if(parse_linkage_area(argc, argv) != 0) Halt(0x64);

  return 0;
}
