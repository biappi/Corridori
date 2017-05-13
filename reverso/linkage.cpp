#include "common.h"
#include "error.h"
#include "linkage.h"
#include "lzr.h"
#include "pascal/system.h"

#include <stdio.h>

pstring_t linkage_area_t::header_() {
  pstring_t ret(this->header, sizeof(this->header));
  return ret;
}

pstring_t linkage_area_t::interface_version_string_() {
  pstring_t ret(this->interface_version_string, sizeof(this->interface_version_string));
  return ret;
}

pstring_t linkage_area_t::language_() {
  pstring_t ret(this->language, sizeof(this->language));
  return ret;
}

char* linkage_area_t::c_ptr() {
  return (char*)this;
}

char linkage_area_t::c_val(uint16_t idx) {
  return ((char*)this)[idx];
}

// Converts a 4 character hex string to a value
uint16_t pointer_string_to_value(pstring_t string) {
  // char local_var1[4];
  char tmp;
  uint16_t ret = 0;
  // PStringOperatorEquals(string, local_var1, 4);

  for (int i = 1; i <= 4; i++) {
    tmp = ((char*)&string)[i];
    if (tmp >= '0' && tmp <= '9') {
      ret += (tmp - '0') << (4 * (4 - i));
    }
    else if (tmp >= 'a' && tmp <= 'f') {
      ret += (tmp - 'a' + 10) << (4 * (4 - i));
    }
    else if (tmp >= 'A' && tmp <= 'F') {
      ret += (tmp - 'A' + 10) << (4 * (4 - i));
    }
  }

  return ret;
}

void copy_line_to_pascal_string(pstring_t &dst, char* src) {
  int i = 0;
  while (i < 255 && src[i++] != 0);
  i--;
  pstring_t local_str;
  if (i > 0) {
    PMove(src, local_str.buffer, i);
  }
  local_str.size = i;
  PStringOperatorEquals(local_str, dst, 0x45);
}

void uppercase_string(pstring_t &dst, const pstring_t &src) {
  pstring_t local_str;
  PStringOperatorEquals(src, local_str, 0xff);
  uint8_t str_sz, loop_idx;
  str_sz = local_str.size;
  if (str_sz >= 1) {
    for (loop_idx = 1; loop_idx <= str_sz; loop_idx++) {
      local_str.c_ptr()[loop_idx] = PCharUpCase(local_str.c_val(loop_idx));
    }
  }
  local_str.c_ptr()[str_sz+1] = 0;
  PStringOperatorEquals(local_str, dst, 0xff);
}

void build_game_paths(const pstring_t &game_dir_path, const pstring_t &another_path) {
  pstring_t game_dir;
  pstring_t another_dir;
  pstring_t upper_str;

  PStringOperatorEquals(game_dir_path, game_dir, 0x45);
  PStringOperatorEquals(another_path, another_dir, 0x45);
  uppercase_string(upper_str, game_dir);
  PStringOperatorEquals(upper_str, game_dir, 0x45);
  uppercase_string(upper_str, another_dir);
  PStringOperatorEquals(upper_str, another_dir, 0x45); // src, dst, trunc

  // ucc_path = game_dir + another_dir + aUcc
  PStringOperatorEquals(upper_str, game_dir); // dst, src
  PStringConcat(upper_str, another_dir); // dst, src
  PStringConcat(upper_str, aUcc);
  PStringOperatorEquals(upper_str, ucc_path, 0x45); // src, dst, trunc

  // img_path = game_dir + another_dir + aImg
  PStringOperatorEquals(upper_str, game_dir);
  PStringConcat(upper_str, another_dir);
  PStringConcat(upper_str, aImg);
  PStringOperatorEquals(upper_str, img_path, 0x45);

  // fil_path = game_dir + another_dir + aFil
  PStringOperatorEquals(upper_str, game_dir);
  PStringConcat(upper_str, another_dir);
  PStringConcat(upper_str, aFil);
  PStringOperatorEquals(upper_str, fil_path, 0x45);

  // mus_path = game_dir + aMus
  PStringOperatorEquals(upper_str, game_dir);
  PStringConcat(upper_str, aMus);
  PStringOperatorEquals(upper_str, mus_path, 0x45);

  // map_path = game_dir + another_dir + aMap
  PStringOperatorEquals(upper_str, game_dir);
  PStringConcat(upper_str, another_dir);
  PStringConcat(upper_str, aMap);
  PStringOperatorEquals(upper_str, map_path, 0x45);

  // sta_path = game_dir + another_dir + aSta
  PStringOperatorEquals(upper_str, game_dir);
  PStringConcat(upper_str, another_dir);
  PStringConcat(upper_str, aSta);
  PStringOperatorEquals(upper_str, sta_path, 0x45);

  // fnt_path = game_dir + aFnt
  PStringOperatorEquals(upper_str, game_dir);
  PStringConcat(upper_str, aFnt);
  PStringOperatorEquals(upper_str, fnt_path, 0x45);

  // lnp_path = game_dir + aLnp
  PStringOperatorEquals(upper_str, game_dir);
  PStringConcat(upper_str, aLnp);
  PStringOperatorEquals(upper_str, lnp_path, 0x45);
}

uint8_t parse_linkage_area(int argc, char** argv) {
  uint8_t ret = 0;
  const pstring_t sColon = pstring_t(":");
  pstring_t temp = pstring_t();
  pstring_t linkage_point_as_string = pstring_t();
  uint16_t link_segment_from_str, link_offset_from_str;
  uint16_t link_seg, link_off;
  uint32_t link;
  uint8_t idx;
  uint8_t interface_version_string;
  pstring_t header = pstring_t();
  pstring_t local_game_dir_path;
  pstring_t local_another_string;

  linkage_area_t *linkage_header;

  if (argc == 2) {
    temp = pstring_t(argv[1], strlen(argv[1]));
    // Source, destination, truncate
    PStringOperatorEquals(temp, linkage_point_as_string, 0xFF);
    if (linkage_point_as_string.size != 9) return ret;
    if (PStringPos(sColon, linkage_point_as_string) != 5) return ret;
    // destination, source, index, count
    PStringCopy(temp, linkage_point_as_string, 1, 4);
    link_segment_from_str = pointer_string_to_value(temp);
    PStringCopy(temp, linkage_point_as_string, 6, 4);
    link_offset_from_str = pointer_string_to_value(temp);
    linkage_area_in_manager_seg = link_segment_from_str;
    linkage_area_in_manager_off = link_offset_from_str;
    link_seg = linkage_area_in_manager_seg;
    link_off = linkage_area_in_manager_off;
    link = ((long)link_seg << 16) + link_off;
    linkage_header = (linkage_area_t*)link;

    PStringPrintln(linkage_header->header_());
    PStringPrintln(linkage_header->interface_version_string_());
    PStringPrintln(linkage_header->language_());
    getchar();

    if (!PStringOperatorMinus(linkage_header->header_(), aSimulmondo)) {
      halt_with_error(EMPTY_STRING, 1, interface_header_error);
    }

    if (!PStringOperatorMinus(linkage_header->interface_version_string_(), a10c)) {
      halt_with_error(EMPTY_STRING, 1, interface_version_error);
    }

    if (linkage_header->c_val(0x55) != 0) {
      halt_with_error(EMPTY_STRING, 1, protocol_error_manager_aracade);
    }

    byte_21859 = 0;
    if (linkage_header->c_val(0x0f) == 1) byte_21859 += 1;
    PStringOperatorEquals(aSwapgame, byte_229D6, 0x45);
    if (linkage_header->c_val(0x25) == byte_229D6.size) {
      halt_with_error(EMPTY_STRING, 1, different_dimension_of_init_string);
    }

    if (byte_21859 == 0) {
      load_arc_lzr(byte_2294A);
    }
    else {
      copy_line_to_pascal_string(local_game_dir_path, linkage_header->line1);
      uppercase_string(local_another_string, local_game_dir_path);
      PStringOperatorEquals(local_another_string, byte_2294A, 0x45);
    }

    if (linkage_header->error_string_size != 0x45) {
      halt_with_error(EMPTY_STRING, 1, different_dimension_of_error_string);
    }

    copy_line_to_pascal_string(local_game_dir_path, linkage_header->line2);
    uppercase_string(local_another_string, local_game_dir_path);
    PStringOperatorEquals(local_another_string, glob_error_msg, 0x45);
    PStringOperatorEquals(aNessunErrore, glob_error_msg, 0x45); // ??? glob_error_msg is overwritten
    PStringOperatorEquals(local_game_dir_path, game_dir_path);
    PStringConcat(local_game_dir_path, aBackslash);
    PStringOperatorEquals(local_another_string, byte_2294A);
    PStringConcat(local_another_string, aBackslash);
    build_game_paths(local_game_dir_path, local_another_string);

    if (linkage_header->conf_data_size != 0x27) {
      halt_with_error(EMPTY_STRING, 1, different_dimension_of_conf_data);
    }

    settings = linkage_header->settings;

    if (linkage_header->sec_vbuf_size != 0xfa00) {
      halt_with_error(EMPTY_STRING, 1, different_dimension_of_secondary_video_buf);
    }

    read_from_keyboard_ISR = linkage_header->read_from_keyboard_ISR;
    if (linkage_header->palette_size != 0x0305) {
      halt_with_error(EMPTY_STRING, 1, different_dimension_of_palette);
    }

    some_pointers.ptr1 = linkage_header->mah1;
    some_pointers.ptr2 = linkage_header->mah2;
    some_pointers.ptr3 = linkage_header->mah3;
    some_pointers.ptr4 = linkage_header->mah4;
    some_pointers.ptr5 = linkage_header->mah5;
    some_pointers.ptr6 = linkage_header->mah6;
    some_pointers.ptr7 = linkage_header->mah7;
    some_pointers.ptr8 = linkage_header->mah8;
    some_pointers.ptr9 = linkage_header->mah9;

    ret = 1;
  }
  return ret;
}
