#include "s21_decimal.h"

int getBit(s21_decimal num, int i) {
  unsigned int mask = 1u << (i % 32);
  return !(!(num.bits[i / 32] & mask));
}

int getBitBig(big_decimal num, int i) {
  unsigned int mask = 1u << (i % 32);
  return !(!(num.bits[i / 32] & mask));
}

int getSign(s21_decimal num) { return !(!(num.bits[3] & 1u << 31)); }

int getScale(s21_decimal num) {
  setSign(&num, 0);
  return num.bits[3] >> 16u;
}

int getMajorBit(s21_decimal num) {
  for (int i = 95; i >= 0; i--)
    if (getBit(num, i)) return i;
  return -1;
}
