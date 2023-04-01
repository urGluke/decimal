#include "s21_decimal.h"

int s21_sub(s21_decimal a, s21_decimal b, s21_decimal *c) {
  s21_decimal zero = {{0}};
  init(c);
  int flag = 0;
  cast_scale(&a, &b);
  int signA = getSign(a), signB = getSign(b);
  if (signA && signB) {  //-1-(-2) = -1+2
    setSign(&a, 0), setSign(&b, 0);
    if (s21_is_less_or_equal(a, b))
      sub_bites(b, a, c);  // -1+2
    else
      sub_bites(a, b, c), setSign(c, 1);  // -2+1
  } else if (!signA && signB) {           // 2-(-1)
    setSign(&b, 0);
    flag = s21_add(a, b, c);
  } else if (signA && !signB) {  //-2 - 1
    setSign(&a, 0), setSign(&b, 0);
    flag = s21_add(a, b, c);
    setSign(c, 1);
  } else {
    if (!s21_is_less(a, b))
      sub_bites(a, b, c);  // 2 - 1
    else
      sub_bites(b, a, c), setSign(c, 1);  // 1 - 2
  }
  if (flag == 1) flag = 2;
  if (compareBits(*c, zero) == EQUAL)
    setScale(c, 0);
  else
    setScale(c, getScale(a));
  return flag;
}

void sub_bites(s21_decimal a, s21_decimal b, s21_decimal *c) {
  int bitA, bitB, zaim = 0;
  for (int i = 0; i < 96; i++) {
    bitA = getBit(a, i);
    bitB = getBit(b, i);
    if (bitA == 1 && bitB == 0) {
      if (zaim == 0)
        setBit(c, i, 1);
      else
        setBit(c, i, 0), zaim = 0;
    }
    if ((bitA == 1 && bitB == 1) || (bitA == 0 && bitB == 0)) {
      if (zaim == 0)
        setBit(c, i, 0);
      else
        setBit(c, i, 1);
    }
    if (bitA == 0 && bitB == 1) {
      if (zaim == 0)
        setBit(c, i, 1), zaim = 1;
      else
        setBit(c, i, 0);
    }
  }
}