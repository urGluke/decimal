#include "s21_decimal.h"
int compareBits(s21_decimal a, s21_decimal b) {
  int f = EQUAL;
  for (int i = 32 * 3 - 1; i >= 0 && !f; i--) {
    if (getBit(a, i) != getBit(b, i)) {
      if (getBit(a, i) < getBit(b, i))
        f = LESS;
      else
        f = BIG;
    }
  }
  return f;
}

int s21_is_less(s21_decimal a, s21_decimal b) {
  cast_scale(&a, &b);
  int f = FALSE;
  if (getSign(a) == getSign(b)) {
    int sign = getSign(a);
    int a_scale = getScale(a);
    int b_scale = getScale(b);
    if (a_scale == b_scale) {
      if (sign == 0) {  //+
        if (compareBits(a, b) == LESS) f = TRUE;
      } else {  //-
        if (compareBits(a, b) == BIG) f = TRUE;
      }
    } else if (a_scale > b_scale && sign == 0) {
      f = TRUE;
    } else if (a_scale < b_scale && sign == 1) {
      f = TRUE;
    }
  } else if (getSign(a) > getSign(b)) {  //-a +b
    f = TRUE;
  }

  return f;
}

int s21_is_equal(s21_decimal a, s21_decimal b) {
  if (!a.bits[0] && !a.bits[1] && !a.bits[2] && !b.bits[0] && !b.bits[1] &&
      !b.bits[2])
    return TRUE;

  cast_scale(&a, &b);

  int CODE = FALSE;
  int sign_a = getSign(a), sign_b = getSign(b);
  int scale_a = getScale(a), scale_b = getScale(b);
  if ((sign_a == sign_b) && (scale_a == scale_b)) {
    CODE = TRUE;
    for (int i = 95; CODE && i >= 0; i--)
      if (getBit(a, i) != getBit(b, i)) CODE = FALSE;
  };
  return CODE;
}

int s21_is_greater(s21_decimal a, s21_decimal b) {
  cast_scale(&a, &b);
  int CODE = FALSE;
  if (s21_is_not_equal(a, b) && !s21_is_less(a, b)) CODE = TRUE;
  return CODE;
}

int s21_is_greater_or_equal(s21_decimal a, s21_decimal b) {
  cast_scale(&a, &b);
  int CODE = FALSE;
  if (s21_is_greater(a, b) || s21_is_equal(a, b)) CODE = TRUE;
  return CODE;
}

int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
  cast_scale(&a, &b);
  int CODE = FALSE;
  if (s21_is_less(a, b) || s21_is_equal(a, b)) CODE = TRUE;
  return CODE;
}

int s21_is_not_equal(s21_decimal a, s21_decimal b) {
  return !s21_is_equal(a, b);
}
