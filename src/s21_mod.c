#include "s21_decimal.h"

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (result == NULL) return 0;
  init(result);
  s21_decimal res = {0};
  s21_decimal mul = {0};
  s21_decimal fraction = {0};
  int err = s21_div(value_1, value_2, &res);
  if (!err) {
    s21_truncate(res, &fraction);
    s21_mul(fraction, value_2, &mul);
    err += s21_sub(value_1, mul, &res);
    *result = res;
  }
  return err;
}