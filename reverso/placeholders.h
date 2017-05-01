#ifndef CORR_PLACEHOLDERS_H
#define CORR_PLACEHOLDERS_H

#include <stdint.h>
#include <stdlib.h>

static void GetIntVec(uint8_t, void (*)()) {};
static void atexit() {};
static void Halt(uint16_t halt_status) {
  exit(halt_status);
}
#endif
