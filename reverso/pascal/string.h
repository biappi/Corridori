#ifndef CORR_PASCAL_STRING_H
#define CORR_PASCAL_STRING_H

#include <stdlib.h>
#include <algorithm>

/* typedef struct pstring_t {
  uint8_t size;
  char* buffer;

  struct pstring_t(const char* str) {
    this->size = strlen(str);
    buffer = (char*)malloc(this->size + 1);
    memset(buffer, 0, this->size + 1);
    memcpy(buffer, str, this->size);
  }

  static struct pstring_t operator_minus() {
    return pstring_t("x");
  };

  static struct pstring_t operator_equals(const pstring_t)

} pstring_t; */


char* PStringConcat(char* &dst, char* &src);
void PStringOperatorEquals(char* &dst, const char* src);
void PStringOperatorEquals(const char* src, char* &dst, size_t truncate);
bool PStringOperatorMinus(const char* op1, const char* op2);
int8_t PStringPos(const char* substr, char* str);
void PStringCopy(char* &dst, char* &src, uint8_t index, size_t count);

#endif
