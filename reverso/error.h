#ifndef CORR_ERROR_H
#define CORR_ERROR_H

#include "common.h"
#include "pascal/string.h"

static pstring_t glob_error_msg;
static uint8_t byte_22CDE;

// These two shouldn't probably be here...
static void(*deinit_1)();
static void(*deinit_2)();

enum error_val {
  not_enough_memory  = 1,
  deallocation_memory  = 2,
  font_color_error  = 3,
  invalid_font_file_type  = 4,
  file_not_found   = 5,
  joystick_calibration_error  = 6,
  different_images_number_file  = 7,
  too_many_bobs_for_background  = 8,
  error_opening_pti_file  = 9,
  error_closing_pti_file  = 0x0A,
  initeffects_error  = 0x0B,
  starteffect_error  = 0x0C,
  parameters_mismatch_calling_rincorsabucata  = 0x0D,
  parameters_mismatch_calling_puocorrere  = 0x0E,
  mouse_init_error  = 0x0F,
  interface_header_error  = 0x10,
  interface_version_error  = 0x11,
  protocol_error_manager_aracade  = 0x12,
  different_number_of_vars  = 0x13,
  different_number_of_switch  = 0x14,
  different_dimension_of_init_string  = 0x15,
  different_dimension_of_error_string  = 0x16,
  different_dimension_of_conf_data  = 0x17,
  different_dimension_of_secondary_video_buf  = 0x18,
  different_dimension_of_palette  = 0x19,
};

const pstring_t error_strings[0x19] = {
  pstring_t("not enough memory"),
  pstring_t("deallocation memory"),
  pstring_t("font color error, "),
  pstring_t("invalid font file type, "),
  pstring_t("file not found "),
  pstring_t("Joystick calibration error"),
  pstring_t("different images number, file "),
  pstring_t("too many bobs for backgrounds"),
  pstring_t("error opening pti file"),
  pstring_t("error closing pti file"),
  pstring_t("InitEffects error"),
  pstring_t("StartEffect error"),
  pstring_t("parameter mismatch calling RincorsaBucata"),
  pstring_t("parameter mismatch calling PuoCorrere"),
  pstring_t("mouse initialization error"),
  pstring_t("interface header error"),
  pstring_t("interface version error"),
  pstring_t("protocol error : MANAGER <--> ARCADE"),
  pstring_t("different number of vars"),
  pstring_t("different number of switch"),
  pstring_t("different dimension of init string"),
  pstring_t("different dimension of error string"),
  pstring_t("different dimension of configuration data"),
  pstring_t("different dimension of secondary video buffer"),
  pstring_t("different dimension of palette")
};

// Here the order of the stuff put on the stack is reversed wrt the order of the params
void halt_with_error(const pstring_t message, int retval, int error_string_id);

#endif
