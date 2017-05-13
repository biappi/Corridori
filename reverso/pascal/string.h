#ifndef CORR_PASCAL_STRING_H
#define CORR_PASCAL_STRING_H

#include "common.h"

#include <string.h>

struct pstring_t {
  uint8_t size;
  char buffer[255];

  pstring_t(const pstring_t &copy) {
    this->size = copy.size;
    memset(this->buffer, 0, 255);
    memcpy(this->buffer, copy.buffer, this->size);
  }

  pstring_t(const char* str) {
    this->size = strlen(str);
    memset(this->buffer, 0, 255);
    memcpy(this->buffer, str, this->size);
  }

  pstring_t(const char* str, uint8_t sz) {
    this->size = sz;
    memset(this->buffer, 0, 255);
    memcpy(this->buffer, str, sz);
  }

  pstring_t() {
    this->size = 0;
  }

  char* c_ptr() {
    return (char*)this;
  }

  char c_val(uint8_t idx) {
    return ((char*)this)[idx];
  }
};

const pstring_t EMPTY_STRING;

void PStringConcat(struct pstring_t &dst, struct pstring_t &src);
void PStringOperatorEquals(struct pstring_t &dst, const struct pstring_t src);
void PStringOperatorEquals(const struct pstring_t src, struct pstring_t &dst, size_t truncate);
bool PStringOperatorMinus(const struct pstring_t op1, const struct pstring_t op2);
uint8_t PStringPos(const struct pstring_t substr, const struct pstring_t str);
void PStringCopy(struct pstring_t &dst, const struct pstring_t &src, uint8_t index, uint8_t count);
void PStringPrint(const struct pstring_t str);
void PStringPrintln(const struct pstring_t str);

char PCharUpCase(char c);
#endif
