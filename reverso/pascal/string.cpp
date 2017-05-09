#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "string.h"

char* PStringConcat(char* &dst, char* &src) {
  char* tmp = (char*)realloc(dst, strlen(dst) + strlen(src) + 1);
  strcpy(tmp + strlen(dst), src);
  return tmp;
}

void PStringOperatorEquals(char* &dst, const char* src) {
  size_t sz = (strlen(src) + 1) * sizeof(char);

  dst = (char*)realloc(dst, sz);
  memcpy(dst, src, sz);
}

void PStringOperatorEquals(const char* src, char* &dst, size_t truncate) {
  char* tmp;
  size_t sz = std::min(truncate + 1, strlen(src) + 1) * sizeof(char);
  dst = (char*)realloc(dst, sz);
  memset(dst, 0, sz);
  memcpy(dst, src, sz-1);
}

int8_t PStringPos(const char* substr, char* str) {
  char* pos = strstr(str, substr);
  if (pos == NULL) return 0;
  return pos - str + 1;
}

void PStringCopy(char* &dst, char* &src, uint8_t index, size_t count) {
  size_t new_size = std::min(count + 1, strlen(src) - index + 1);
  char *tmp = (char*)realloc(dst, new_size);
  strncpy(tmp, src + index, new_size);
  tmp[new_size-1] = 0;
  dst = tmp;
}

// return value a bit misleading but kept this way for compatibility with the asm
// false -> the two strings are equal
// true -> the two strings are different
// Probably should replace everything with a strcmp but not sure yet about the semantics of this func
bool PStringOperatorMinus(const char* op1, const char* op2) {
  size_t sz = std::min(strlen(op1+1), strlen(op2+1));
  unsigned int i = 1;
  while (i < sz && op1[i] == op2[i]) i++;

  if (i < sz)
    return true;
  else
    return strlen(op1) != strlen(op2);
}
