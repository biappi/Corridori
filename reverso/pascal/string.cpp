#include "common.h"
#include "pascal/string.h"

#include <algorithm>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void PStringConcat(struct pstring_t &dst, struct pstring_t &src) {
  memcpy(dst.buffer + dst.size, src.buffer, std::min(255, dst.size + src.size));
  dst.size = std::min(255, dst.size + src.size);
}

void PStringOperatorEquals(struct pstring_t &dst, const struct pstring_t src) {
  memset(&dst, 0, sizeof(struct pstring_t));
  memcpy(&dst, &src, dst.size + 1);
}

void PStringOperatorEquals(const struct pstring_t src, struct pstring_t &dst, size_t truncate) {
  size_t sz = std::min(truncate, (size_t)src.size);
  memset(&dst, 0, sizeof(struct pstring_t));
  memcpy(&dst, &src, sz + 1);
  dst.size = sz;
}

uint8_t PStringPos(const struct pstring_t substr, const struct pstring_t str) {
  const char* pos = strstr(str.buffer, substr.buffer);
  if (pos == NULL) return 0;
  return pos - str.buffer + 1;
}

void PStringCopy(struct pstring_t &dst, const struct pstring_t &src, uint8_t index, uint8_t count) {
  if (index == 0) index = 1;
  if (src.size < index) dst.size = 0;

  size_t cnt = std::min((int)count, src.size - index + 1);
  memcpy(dst.buffer, src.buffer + index - 1, cnt);
  dst.size = cnt;
}

// return value a bit misleading but kept this way for compatibility with the asm
// false . the two strings are equal
// true . the two strings are different
// Probably should replace everything with a strcmp but not sure yet about the semantics of this func
bool PStringOperatorMinus(const struct pstring_t op1, const struct pstring_t op2) {
  printf("pstringoperatorminus\n");
  PStringPrintln(op1);
  PStringPrintln(op2);
  printf("%d %d\n", op1.size, op2.size);

  size_t sz = std::min((int)op1.size, (int)op2.size);
  unsigned int i = 1;
  while (i < sz && op1.buffer[i-1] == op2.buffer[i-1]) i++;

  printf("i: %d\n", i);
  printf("ritorno %d\n", i >= sz || (op1.size == op2.size));

  if (i < sz)
    return false;
  else
    return op1.size == op2.size;
}

void PStringPrint(struct pstring_t str) {
  fwrite(str.buffer, str.size, 1, stdout);
  fflush(stdout);
}

void PStringPrintln(struct pstring_t str) {
  const char* newline = "\n";
  fwrite(str.buffer, str.size, 1, stdout);
  fwrite(newline, 1, 1, stdout);
  fflush(stdout);
}

char PCharUpCase(char c) {
  if (c >= 'a' && c <= 'z')
    return c - 'a' + 'A';
  return c;
}
