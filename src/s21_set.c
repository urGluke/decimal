#include "s21_decimal.h"

void setBit(s21_decimal* num, int i, int result) {
  if (result == 1) {
    num->bits[i / 32] |= (1u << i % 32);
  } else if (result == 0) {
    num->bits[i / 32] &= ~(1u << i % 32);
  }
}
void setBitBig(big_decimal* num, int i, int result) {
  if (result == 1) {
    num->bits[i / 32] |= (1u << i % 32);
  } else if (result == 0) {
    num->bits[i / 32] &= ~(1u << i % 32);
  }
}

void setSign(s21_decimal* num, int sign) {
  if (sign == 1) {
    num->bits[3] |= 1u << 31;
  } else {
    num->bits[3] &= ~(1u << 31);
  }
}

void setScale(s21_decimal* num, unsigned int result) {
  int flag = 0;
  if (getSign(*num)) {
    flag = 1;
  }

  num->bits[3] = result << 16u;

  if (flag) {
    setSign(num, 1);
  }
}

void init(s21_decimal* a) {
  for (int i = 0; i < 32 * 4; i++) setBit(a, i, 0);
}

int reverseBit(int a, int i) { return (a ^ (1 << i)); }