#include "s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  init(result);
  short err = 0, sign_1 = getSign(value_1), sign_2 = getSign(value_2);
  unsigned int scale = 0;
  cast_scale(&value_1, &value_2);
  big_decimal val_1 = {{value_1.bits[LOW], value_1.bits[MID],
                        value_1.bits[HIGH], 0, 0, 0}},
              val_2 = {{value_2.bits[LOW], value_2.bits[MID],
                        value_2.bits[HIGH], 0, 0, 0}},
              res = {0};
  scale =
      comparison_scale(&val_1, &val_2, getScale(value_1), getScale(value_2));
  setSign(&value_1, 0);
  setSign(&value_2, 0);
  int sign_res = 0;
  if (!sign_1 && !sign_2) {
    addBinBig(val_1, val_2, &res);
  } else if (!sign_1 && sign_2) {
    if (s21_is_less(value_1, value_2)) {
      sub_bits_big(val_2, val_1, &res);
      sign_res = 1;
    } else if (!s21_is_equal(value_1, value_2)) {
      sub_bits_big(val_1, val_2, &res);
    }
  } else if (sign_1 && !sign_2) {
    if (s21_is_greater(value_1, value_2)) {
      sub_bits_big(val_1, val_2, &res);
      sign_res = 1;
    } else if (!s21_is_equal(value_1, value_2)) {
      sub_bits_big(val_2, val_1, &res);
    }
  } else {
    addBinBig(val_1, val_2, &res);
    sign_res = 1;
  }
  err = check_max(res, scale, sign_res);
  if (result) {
    if (!err) {
      init(result);
      setScale(result, scale);
      setSign(result, sign_res);
      reverse_normalization(res, result);
      while (is_divisible_by_10(*result) && getScale(*result) != 0) {
        div10(result);
        setScale(result, getScale(*result) - 1);
      }
    }
  }
  return err;
}

int add_bytes(s21_decimal value_1, s21_decimal value_2, s21_decimal *c) {
  s21_decimal result = {0};
  int tmp = 0;
  for (int i = 0; i < 97; i++) {
    int left_dec = getBit(value_1, i);
    int right_dec = getBit(value_2, i);

    if (!right_dec && !left_dec && tmp) {
      setBit(&result, i, 1);
      tmp = 0;
    }
    if (right_dec && left_dec) {
      if (tmp == 1) {
        setBit(&result, i, 1);
      }
      tmp = 1;
    }
    if (right_dec ^ left_dec) {
      if (!tmp) setBit(&result, i, 1);
    }
  }
  if (getBit(result, 96)) return 1;
  *c = result;
  return 0;
}