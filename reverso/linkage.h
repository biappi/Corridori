#ifndef CORR_LINKAGE_H
#define CORR_LINKAGE_H

#include "common.h"
#include "keyboard.h"
#include "pascal/string.h"

static uint16_t linkage_area_in_manager_seg;
static uint16_t linkage_area_in_manager_off;
static const pstring_t aGame_dir("GAME_DIR");
static const pstring_t aSimulmondo("SIMULMONDO");
static const pstring_t a10c("10c");
static const pstring_t aSwapgame("SWAPGAME\\");
static const pstring_t aNessunErrore("nessun errore\0");
static const pstring_t aBackslash("\\");
static const pstring_t aUcc("UCC\\");
static const pstring_t aImg("IMG\\");
static const pstring_t aFil("FIL\\");
static const pstring_t aMus("MUS\\");
static const pstring_t aMap("MAP\\");
static const pstring_t aSta("STA\\");
static const pstring_t aFnt("FNT\\");
static const pstring_t aLnp("LNP\\");
static pstring_t game_dir_path;

static uint8_t byte_21859;
static pstring_t byte_229D6;
static pstring_t byte_2294A;
static char* settings;

static pstring_t ucc_path;
static pstring_t img_path;
static pstring_t fil_path;
static pstring_t mus_path;
static pstring_t map_path;
static pstring_t sta_path;
static pstring_t fnt_path;
static pstring_t lnp_path;

typedef struct linkage_area_t {
  char header[10]; // 0x00 - 0x09
  char interface_version_string[3]; // 0x0A - 0x0C
  char language[2]; // 0x0D - 0x0E
  char boh1[4]; // 0x0F - 0x12
  uint16_t conf_data_size; // 0x13 - 0x14
  char* settings; // 0x15 - 0x18
  uint16_t sec_vbuf_size; // 0x19 - 0x1A
  void* read_from_keyboard_ISR; // 0x1B - 0x1E
  uint16_t palette_size; // 0x1F - 0x20
  char boh2[6]; // 0x21 - 0x26
  char* line1;  // 0x27 - 0x2A
  uint16_t error_string_size; // 0x2B - 0x2C
  char* line2; // 0x2D - 0x30
  void* mah1; // 0x31 - 0x34
  void* mah2; // 0x35 - 0x38
  void* mah3; // 0x39 - 0x3C
  void* mah4; // 0x3D - 0x40
  void* mah5; // 0x41 - 0x44
  void* mah6; // 0x45 - 0x48
  void* mah7; // 0x49 - 0x4C
  void* mah8; // 0x4D - 0x50
  void* mah9; // 0x51 - 0x54

  pstring_t header_();
  pstring_t interface_version_string_();
  pstring_t language_();
  char* c_ptr();
  char c_val(uint16_t idx);
} linkage_area_t;

typedef struct some_pointers_t {
  void* ptr1;
  void* ptr2;
  void* ptr3;
  void* ptr4;
  void* ptr5;
  void* ptr6;
  void* ptr7;
  void* ptr8;
  void* ptr9;
} some_pointers_t;

static some_pointers_t some_pointers;

uint16_t pointer_string_to_value(pstring_t string);
void copy_line_to_pascal_string(void* line, pstring_t &out);
uint8_t parse_linkage_area(int argc, char** argv);
void build_game_paths(const pstring_t &game_dir_path, const pstring_t &another_path);

#endif
