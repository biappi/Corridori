#include "error.h"
#include "placeholders.h"

void halt_with_error(const pstring_t message, int retval, int error_string_id) {
  pstring_t string_local = pstring_t();
  pstring_t final_error_msg = pstring_t();

  PStringOperatorEquals(message, string_local, 0x45);
  if (error_string_id == 0) PStringOperatorEquals(string_local, glob_error_msg, 0x45);
  else {
    PStringOperatorEquals(final_error_msg, error_strings[error_string_id-1]);
    PStringConcat(final_error_msg, string_local);
    PStringOperatorEquals(final_error_msg, glob_error_msg, 0x45);
  }
  byte_22CDE = 1;
  deinit_1();
  deinit_2();
  Halt(retval);
}
