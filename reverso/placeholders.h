#ifndef CORR_PLACEHOLDERS_H
#define CORR_PLACEHOLDERS_H

//#include <stdint.h>
typedef unsigned char uint8_t;

#include <stdlib.h>

static void GetIntVec(uint8_t, void (*)()) {};
static void SetIntVec(uint8_t, void (*)()) {};
static void atexit_func() {

};
static void set_atexit(void (*)()) {};

static void Halt(int halt_status) {
  exit(halt_status);
}
#endif
