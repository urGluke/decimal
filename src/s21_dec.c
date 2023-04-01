#include "s21_decimal.h"
int s21_div(s21_decimal a, s21_decimal b, s21_decimal *c) {
  init(c);
  int err = 0, scale = 0;
  if (c == NULL) return err;
  s21_decimal zero = {{0}};
  if (s21_is_equal(b, zero)) err = 3;

  int sign = getSign(a) ^ getSign(b);
  big_decimal res = {0};
  if (err != 3) {
    scale = division(a, b, &res);

    if (is_overflowed(&res) && err != 3) {
      if (sign)
        err = 2;
      else
        err = 1;
    }
  }
  if (!err) {
    c->bits[0] = res.bits[0];
    c->bits[1] = res.bits[1];
    c->bits[2] = res.bits[2];
    setScale(c, scale);
    setSign(c, sign);
  }

  return err;
}
void comp_scale(big_decimal *num_1, big_decimal *num_2, s21_decimal *a,
                s21_decimal *b) {
  if (getScale(*a) > getScale(*b)) {
    normalization(num_2, getScale(*a) - getScale(*b));
    setScale(b, getScale(*a));
  } else if (getScale(*a) < getScale(*b)) {
    normalization(num_1, getScale(*b) - getScale(*a));
    setScale(a, getScale(*b));
  }
}
int s21_abs(int number) { return (number < 0) ? number * (-1) : number; }

big_decimal bin_div(big_decimal v1, big_decimal v2) {
  big_decimal l = {0}, r = v1, mid = {0};

  big_decimal tmp = {0};
  big_decimal one = {{1, 0, 0, 0, 0, 0}};
  big_decimal res = {0};
  while (big_dec_compare(l, r) != BIG) {
    sub_bits_big(r, l, &tmp);

    shift_big_decimal(&tmp, -1);
    addBinBig(l, tmp, &mid);
    tmp = big_mult(v2, mid);
    if (big_dec_compare(tmp, v1) == BIG) {
      sub_bits_big(mid, one, &r);
    } else {
      res = mid;
      ;
      addBinBig(mid, one, &l);
    }
  }
  return res;
}

void upscale(big_decimal *v1, int diff1) {
  big_decimal ten = {{10, 0, 0, 0, 0, 0}};

  while (diff1) {
    *v1 = big_mult(*v1, ten);
    diff1--;
  }
}
int find_down(big_decimal res, big_decimal celoe) {
  int scale = 0;
  int down = 28;
  while (big_dec_compare(res, celoe) != EQUAL && down >= 0) {
    div10_big_dec(&res);
    scale++;
    down--;
  }
  return scale;
}
int division(s21_decimal a, s21_decimal b, big_decimal *c) {
  big_decimal val_1 = {{a.bits[0], a.bits[1], a.bits[2], 0, 0, 0}},
              val_2 = {{b.bits[0], b.bits[1], b.bits[2], 0, 0, 0}}, celoe = {0},
              v1cpy = val_1, v2cpy = val_2;
  s21_decimal one = {{1, 0, 0, 0}};
  big_decimal big_one = {{1, 0, 0, 0, 0, 0}};
  int ans = getScale(a);

  if (!s21_is_equal(b, one)) {
    int scale1 = getScale(a);
    int scale2 = getScale(b);
    int up = (scale1 != scale2) ? 28 + s21_abs(scale1 - scale2) - 1 : 29;
    comp_scale(&v1cpy, &v2cpy, &a, &b);

    celoe = bin_div(v1cpy, v2cpy);
    if (scale1 != scale2) {
      if (s21_is_less(a, b) && !s21_is_equal(b, one)) {
        upscale(&val_1, up);
      } else if (!s21_is_equal(b, one))
        upscale(&val_1, 29);
    } else if (!s21_is_equal(b, one))
      upscale(&val_1, 28);
    *c = bin_div(val_1, val_2);
    int scale = find_down(*c, celoe);
    int down = scale <= 28 ? scale : 28;
    while ((is_overflowed(c) && down > 0) ||
           (is_divisible_by_10_overflow(*c) && down > 0)) {
      bank_round(*c, c);
      down--;
    }
    ans = find_scale(*c, celoe);
  } else {
    *c = bin_div(val_1, big_one);
  }
  return ans;
}
int find_scale(big_decimal res, big_decimal celoe) {
  int scale = 0;
  int down = 28;
  while (big_dec_compare(res, celoe) != EQUAL && down > 0) {
    div10_big_dec(&res);
    scale++;
    down--;
  }
  return scale;
}
big_decimal shift_big_decimal(big_decimal *decimal, int shift) {
  if (shift > 0) {
    for (int i = 0; i < shift; i++) {
      int bit1 = getBitBig(*decimal, 31);
      int bit2 = getBitBig(*decimal, 63);
      int bit3 = getBitBig(*decimal, 95);
      int bit4 = getBitBig(*decimal, 127);
      int bit5 = getBitBig(*decimal, 159);
      decimal->bits[5] <<= 1;
      decimal->bits[4] <<= 1;
      decimal->bits[3] <<= 1;
      decimal->bits[2] <<= 1;
      decimal->bits[1] <<= 1;
      decimal->bits[0] <<= 1;
      setBitBig(decimal, 32, bit1);
      setBitBig(decimal, 64, bit2);
      setBitBig(decimal, 96, bit3);
      setBitBig(decimal, 128, bit4);
      setBitBig(decimal, 160, bit5);
    }
  } else {
    for (int i = 0; i < shift * (-1); i++) {
      int bit1 = getBitBig(*decimal, 32);
      int bit2 = getBitBig(*decimal, 64);
      int bit3 = getBitBig(*decimal, 96);
      int bit4 = getBitBig(*decimal, 128);
      int bit5 = getBitBig(*decimal, 160);
      decimal->bits[5] >>= 1;
      decimal->bits[4] >>= 1;
      decimal->bits[3] >>= 1;
      decimal->bits[2] >>= 1;
      decimal->bits[1] >>= 1;
      decimal->bits[0] >>= 1;
      setBitBig(decimal, 31, bit1);
      setBitBig(decimal, 63, bit2);
      setBitBig(decimal, 95, bit3);
      setBitBig(decimal, 127, bit4);
      setBitBig(decimal, 159, bit5);
    }
  }
  return *decimal;
}
void shift_decimal(s21_decimal *decimal, int shift) {
  if (shift > 0) {
    for (int i = 0; i < shift; i++) {
      int bit1 = getBit(*decimal, 31);
      int bit2 = getBit(*decimal, 63);
      decimal->bits[2] <<= 1;
      decimal->bits[1] <<= 1;
      decimal->bits[0] <<= 1;
      setBit(decimal, 32, bit1);
      setBit(decimal, 64, bit2);
    }
  } else {
    for (int i = 0; i < shift * (-1); i++) {
      int bit1 = getBit(*decimal, 32);
      int bit2 = getBit(*decimal, 64);
      decimal->bits[2] >>= 1;
      decimal->bits[1] >>= 1;
      decimal->bits[0] >>= 1;
      setBit(decimal, 31, bit1);
      setBit(decimal, 63, bit2);
    }
  }
}

s21_decimal div_int(s21_decimal a, s21_decimal b, s21_decimal *c) {
  s21_decimal tmp = {0};
  s21_decimal zero = {0};
  if (s21_is_less(a, b)) return a;
  init(c);
  int majorA = getMajorBit(a), majorB = getMajorBit(b);
  int shift_bite = majorA - majorB;
  shift_decimal(&b, shift_bite);
  if (compareBits(a, b) == LESS) shift_decimal(&b, -1), shift_bite--;
  s21_sub(a, b, &tmp);
  c->bits[0] = 1;
  while (shift_bite > 0) {
    shift_decimal(&b, -1), shift_bite--;
    shift_decimal(c, 1);
    if (compareBits(tmp, b) == LESS) continue;
    if (compareBits(tmp, zero) != EQUAL) s21_sub(tmp, b, &tmp), setBit(c, 0, 1);
  }
  return tmp;
}

void div_ten(s21_decimal *a) {
  s21_decimal tmp = {{10, 0, 0, 0}};
  s21_decimal tmp2 = *a;
  div_int(tmp2, tmp, a);
}

int getMajorMax(s21_decimal a, s21_decimal b) {
  int majorA = getMajorBit(a), majorB = getMajorBit(b);
  int majorMax;
  if (majorA > majorB)
    majorMax = majorA;
  else
    majorMax = majorB;
  return majorMax;
}

void cast_scale(s21_decimal *a, s21_decimal *b) {
  int scale_a = getScale(*a);
  int scale_b = getScale(*b);
  int scale_cast;
  while (scale_a != scale_b) {
    if (getMajorMax(*a, *b) + 3 > 96) {
      if (scale_a > scale_b)
        div_ten(a), scale_a--;
      else
        div_ten(b), scale_b--;
    } else {
      if (scale_a > scale_b)
        mult_ten(b), scale_b++;
      else
        mult_ten(a), scale_a++;
    }
  }
  scale_cast = scale_a;
  setScale(a, scale_cast);
  setScale(b, scale_cast);
}
