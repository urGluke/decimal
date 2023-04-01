#include "s21_decimal.h"

int s21_mul(s21_decimal a, s21_decimal b, s21_decimal* c) {
  init(c);

  int scaleA = getScale(a), scaleB = getScale(b);
  int signA = getSign(a), signB = getSign(b);
  if (scaleA + scaleB > 28) return 1;
  setScale(c, scaleA + scaleB);
  if (signA ^ signB) setSign(c, 1);
  return mult_int(a, b, c);
}

int mult_int(s21_decimal a, s21_decimal b, s21_decimal* c) {
  int err = 0, scale = getScale(*c), sign = getSign(*c);
  big_decimal tmp = {{0}};
  big_decimal res = {{0}};
  big_decimal bb = {{b.bits[0], b.bits[1], b.bits[2], 0, 0, 0}};
  big_decimal t = bb;
  s21_decimal zero = {{0}};
  for (int i = 0; i < 96; i++) {
    if (getBit(a, i)) {
      big_init(&tmp);
      tmp = res;
      addBinBig(tmp, shift_big_decimal(&bb, i), &res);
      bb = t;
    }
  }
  err = check_max(res, scale, sign);
  if (c) {
    if (!err) {
      init(c);
      setScale(c, scale);
      setSign(c, sign);

      reverse_normalization(res, c);

      if (compareBits(*c, zero) == EQUAL) {
        if (getScale(*c) >= 28) err = 1;
        init(c);
      } else {
        while (is_divisible_by_10(*c) && getScale(*c) != 0) {
          div10(c);
          setScale(c, getScale(*c) - 1);
        }
      }
    }
  }

  return err;
}
int mult_ten(s21_decimal* a) {
  s21_decimal tmp = *a;
  s21_decimal tmp2 = *a;  //проверять мажорный бит, чтоб не переполняться
  int scale = getScale(*a);
  int sign = getSign(*a);
  init(a);
  shift_decimal(&tmp, 3);
  shift_decimal(&tmp2, 1);
  if (add_bytes(tmp, tmp2, a)) {
    return 1;
  }
  setScale(a, scale);
  setSign(a, sign);
  return 0;
}