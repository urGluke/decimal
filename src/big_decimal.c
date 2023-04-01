#include "s21_decimal.h"

int big_dec_compare(big_decimal a, big_decimal b) {
  int f = EQUAL;
  for (int i = 32 * 6 - 1; i >= 0 && !f; i--) {
    if (getBitBig(a, i) != getBitBig(b, i)) {
      if (getBitBig(a, i) < getBitBig(b, i))
        f = LESS;
      else
        f = BIG;
    }
  }
  return f;
}

int check_max(big_decimal val_1, int scale, int sign) {
  big_decimal max = {{MAX_DEC, MAX_DEC, MAX_DEC, 0, 0, 0}};
  int err = 0;
  for (int i = 0; i < scale; i++) {
    div10_big_dec(&val_1);
  }
  if (big_dec_compare(val_1, max) == BIG) {
    err = sign ? 2 : 1;
  }
  return err;
}

int sub_bin_big(big_decimal value_1, big_decimal value_2, big_decimal *result) {
  int err;
  big_decimal zer;
  big_init(&zer);
  if (big_dec_compare(value_2, zer) != EQUAL) {
    for (int i = 0; i < 192; i++)
      value_2.bits[i / 32] = reverseBit(value_2.bits[i / 32], i);
    value_2.bits[LOW]++;
  }
  err = addBinBig(value_1, value_2, result);
  return err;
}
void reverse_normalization(big_decimal value, s21_decimal *result) {
  while (value.bits[3] || value.bits[4] || value.bits[5] ||
         (getScale(*result) > 28)) {
    bank_round(value, &value);
    result->bits[3] -= 1 << 16;
  }
  result->bits[0] = value.bits[0];
  result->bits[1] = value.bits[1];
  result->bits[2] = value.bits[2];
}

int div10_big_dec(big_decimal *n) {
  unsigned int mod = 0;
  long long int tmp;
  for (int i = 5; i >= 0; i--) {
    tmp = (unsigned)n->bits[i] + ((long long)(mod) << 32);
    mod = tmp % 10;
    n->bits[i] = tmp / 10;
  }
  return mod;
}

void big_init(big_decimal *val) {
  val->bits[0] = 0;
  val->bits[1] = 0;
  val->bits[2] = 0;
  val->bits[3] = 0;
  val->bits[4] = 0;
  val->bits[5] = 0;
}
void bank_round(big_decimal value, big_decimal *result) {
  if (result) {
    big_init(result);
    big_decimal mod1, mod2;
    big_init(&mod1);
    big_init(&mod2);
    big_decimal one = {{1, 0, 0, 0, 0, 0}};
    mod10(value, &mod1);
    div10_big_dec(&value);
    if (mod1.bits[LOW] > 5) {
      addBinBig(value, one, &value);
    } else if (mod1.bits[LOW] == 5) {
      mod10(value, &mod2);
      if (mod2.bits[LOW] % 2 != 0) addBinBig(value, one, &value);
    }
  }
  *result = value;
}

void sub_bits_big(big_decimal a, big_decimal b, big_decimal *c) {
  int bitA, bitB, tmp = 0;
  for (int i = 0; i < 6 * 32; i++) {
    bitA = getBitBig(a, i);
    bitB = getBitBig(b, i);
    if (bitA == 1 && bitB == 0) {
      if (tmp == 0)
        setBitBig(c, i, 1);
      else
        setBitBig(c, i, 0), tmp = 0;
    }
    if ((bitA == 1 && bitB == 1) || (bitA == 0 && bitB == 0)) {
      if (tmp == 0)
        setBitBig(c, i, 0);
      else
        setBitBig(c, i, 1);
    }
    if (bitA == 0 && bitB == 1) {
      if (tmp == 0)
        setBitBig(c, i, 1), tmp = 1;
      else
        setBitBig(c, i, 0);
    }
  }
}

void mod10(big_decimal value, big_decimal *mod) {
  big_decimal ten = {{10, 0, 0, 0, 0, 0}};
  big_decimal tmp = value;
  if (mod) {
    div10_big_dec(&tmp);
    tmp = big_mult(tmp, ten);
    sub_bin_big(value, tmp, mod);
  }
}
big_decimal big_mult(big_decimal value_1, big_decimal value_2) {
  big_decimal tmp = {{0, 0, 0, 0, 0, 0}};
  big_decimal res = {{0, 0, 0, 0, 0, 0}};
  big_decimal b = value_2;
  for (int i = 0; i < 6 * 32; i++) {
    if (getBitBig(value_1, i)) {
      big_init(&tmp);
      tmp = res;
      addBinBig(tmp, shift_big_decimal(&value_2, i), &res);
      value_2 = b;
    }
  }
  return res;
}
int addBinBig(big_decimal value_1, big_decimal value_2, big_decimal *result) {
  short error = 0, tmp = 0;
  int term1, term2;
  big_init(result);
  for (int j = 0; j < 6; j++) {
    for (int i = 0; i < 32; i++) {
      term1 = getBitBig(value_1, j * 32 + i);
      term2 = getBitBig(value_2, j * 32 + i);
      if (term1 && term2) {
        if (tmp) {
          setBitBig(result, j * 32 + i, 1);
        } else {
          tmp = 1;
        }
      } else if (term1 ^ term2) {
        if (!tmp) {
          setBitBig(result, j * 32 + i, 1);
        }
      } else if (tmp) {
        setBitBig(result, j * 32 + i, 1);
        tmp = 0;
      }
    }
    if (tmp && j == 5) error = 1;
  }

  return error;
}
int div10(s21_decimal *n) {
  unsigned int mod = 0;
  long long int tmp;
  for (int i = 2; i >= 0; --i) {
    tmp = (unsigned)n->bits[i] + ((long long)(mod) << 32);
    mod = tmp % 10;
    n->bits[i] = tmp / 10;
  }
  return mod;
}
int div10_overflow(big_decimal *n) {
  unsigned int mod = 0;
  long long int tmp;
  for (int i = 5; i >= 0; --i) {
    tmp = (unsigned)n->bits[i] + ((long long)(mod) << 32);
    mod = tmp % 10;
    n->bits[i] = tmp / 10;
  }
  return mod;
}
int is_divisible_by_10(s21_decimal n) {
  s21_decimal tmp = n;
  return !div10(&tmp);
}
int is_divisible_by_10_overflow(big_decimal n) {
  big_decimal tmp = {0};
  tmp = n;
  return !div10_overflow(&tmp);
}
int is_overflowed(big_decimal *number) {
  return (number->bits[3] || number->bits[4] || number->bits[5]);
}
int comparison_scale(big_decimal *num_1, big_decimal *num_2,
                     unsigned int scale_1, unsigned int scale_2) {
  int flag = 0;
  if (scale_1 > scale_2) {
    normalization(num_2, scale_1 - scale_2);
    flag = scale_2 = scale_1;
  } else if (scale_1 < scale_2) {
    normalization(num_1, scale_2 - scale_1);
    flag = scale_1 = scale_2;
  } else {
    flag = scale_1;
  }
  return flag;
}

void normalization(big_decimal *result, short scale) {
  big_decimal ten = {{10, 0, 0, 0, 0, 0}};
  for (int i = 0; i < scale; i++) {
    *result = big_mult(*result, ten);
  }
}