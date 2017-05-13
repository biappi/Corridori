#include "common.h"
#include "pascal/system.h"

#include <string.h>

void PMove(const void* src, void* dst, uint32_t cnt) {
  memcpy(dst, src, cnt);
}
