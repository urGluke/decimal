#include "s21_decimal.h"
#define ACCURACY 1000000.0

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  if (dst == NULL) return 1;
  int CODE = CONVERTORS_ERROR;
  init(dst);

  long double fl = src;
  int sign = 0;
  if (fl < 0) {
    sign = 1;
    fl *= -1;
  }
  if (fl < 1e-28) {
    s21_from_int_to_decimal(0, dst);
    if (fl == 0.0) CODE = CONVERTORS_OK;
  } else if (fl <= (long double)7.9228162514264337593543950335e28 &&
             !isinf(fl)) {
    CODE = CONVERTORS_OK;
    int scale = 0;
    for (; fl > ACCURACY; fl /= 10, scale--) {
    }
    for (long double left = 0;
         fl < ACCURACY && modfl(fl, &left) > 0 && scale < 28;
         fl *= 10, scale++) {
    }
    fl = roundl(fl);  //к ближ целому
    if (scale < 0) {
      fl *= powl(10, -scale);
      scale = 0;
    }
    setSign(dst, sign);
    setScale(dst, scale);
    for (int i = 0; i < 96; i++, fl = floorl(fl / 2)) {
      setBit(dst, i, fmodl(fl, 2));
    }
  }
  return CODE;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  if (dst == NULL) return 1;
  *dst = 0;
  int CODE = CONVERTORS_OK;
  long double tmp = 0;
  for (int i = 0; i < 96; i++) {
    if (getBit(src, i)) {
      tmp += pow(2, i);
    }
  }
  int scale = getScale(src);
  while (scale != 0) {
    tmp /= 10;
    scale -= 1;
  }
  if (getSign(src) == 1) tmp *= -1;
  *dst = tmp;
  return CODE;
}