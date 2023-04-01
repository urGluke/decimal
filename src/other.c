#include <string.h>

#include "s21_decimal.h"
int s21_negate(s21_decimal value, s21_decimal *result) {
  int CODE = OK;
  if (result) {
    *result = value;
    if (getSign(value)) {
      setSign(result, 0);
    } else {
      setSign(result, 1);
    }
  } else {
    CODE = ERROR_CALC;
  }
  return CODE;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  if (result == NULL) return 1;
  for (int i = 0; i < 4; i += 1) result->bits[i] = 0;
  int sign = getSign(value);
  int scale = getScale(value);
  s21_decimal value1 = value;
  // s21_decimal result1={{0,0,0,0}};

  value1.bits[3] = 0;

  if (scale == 0) {
    for (int i = 0; i < 4; i += 1) result->bits[i] = value.bits[i];
  } else {
    for (int i = 0; i < scale - 1; i++) {
      unsigned long long a = value1.bits[2];

      for (int j = 2; j >= 0; j--) {
        value1.bits[j] = a / 10;
        a = (a % 10) * (S21_MAX_UINT) + value1.bits[j - 1];
      }
    }

    if (value1.bits[0] % 10 < 5) {
      s21_truncate(value, result);
    } else {
      s21_truncate(value, result);
      result->bits[0] += 1;
    }

    setScale(&value, (getScale(value) - scale));
    setSign(result, sign);
  }
  return 0;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  if (result == NULL) return 1;
  for (int i = 0; i < 4; i += 1) result->bits[i] = 0;
  int sign = getSign(value);
  int scale = getScale(value);

  value.bits[3] = 0;
  if (scale == 0)
    *result = value;
  else {
    for (int i = 0; i < scale; i++) {
      unsigned long long a = value.bits[2];
      for (int j = 2; j >= 0; j--) {
        value.bits[j] = a / 10;  //каждый бит делим на 10
        a = (a % 10) * (S21_MAX_UINT) +
            value
                .bits[j - 1];  //поскольку у нас биты хранятся в инте, то
                               //остаток от деления мы прибавляем к предыдущему,
                               //домножая на различие в степенях двойки
      }
    }
    setScale(&value, 0);
    *result = value;
  }
  setSign(result, sign);
  return 0;
}

int s21_floor(s21_decimal a, s21_decimal *res) {
  if (res == NULL) return 1;
  s21_decimal one = {{1, 0, 0, 0}};
  s21_truncate(a, res);
  if (getSign(a)) add_bytes(*res, one, res), setSign(res, 1);
  return 0;
}