#include "s21_decimal.h"
#define aa 21
#define bb 4

/*int main() {
    s21_decimal a = {{aa,0,UINT_MAX,0}}, b={{bb,bb,1,0}}, rez={0};
    setScale(&a, 0); setScale(&b, 5);
    printDecAndBin(a);printDecAndBin(b);
    printf("\n");
    division(a,b,&rez);
    //printf("\n%d\n", add_bytes(a, b, &rez));
    printDecAndBin(rez);
    return 0;
}*/

char* printBin(s21_decimal num) {
  char* number = (char*)calloc(97, sizeof(char));

  char* str3 = toBinary(num.bits[2]);
  // printf("bits[2] - %s\n", str3);
  strcat(number, str3);
  free(str3);

  char* str2 = toBinary(num.bits[1]);
  // printf("bits[1] - %s\n", str2);
  strcat(number, str2);
  free(str2);

  char* str1 = toBinary(num.bits[0]);
  // printf("bits[0] - %s\n", str1);
  strcat(number, str1);
  free(str1);

  printf("binary  -> %-10s\n", number);

  return number;
}

void printDecAndBin(s21_decimal num) {
  char rez[MAX_NUM + 1] = "";
  char* ptr = NULL;
  char* number = printBin(num);
  char stepen[MAX_NUM + 1];
  coup(number);

  char* str4 = toBinary(num.bits[3]);
  // printf("bits[3] -> %-10s\n", str4);
  /*if (str4[0] == '1')   printf("sign    -> -\n");
     else printf("sign    -> +\n");
  char step[10] = {0};
  int j = 0;
  for (int i = 8; i <= 15; i++) {
      step[j] = str4[i]; j++;
  }*/
  free(str4);
  // printf("bin step - %s\n", step);
  int mant = getScale(num);
  printf("scale - %d\n", mant);
  for (int i = 0; i < (int)strlen(number); i++) {
    if (i == 0 && number[i] == '1') {
      rez[0] = '1';
      continue;
    }
    if (number[i] == '1') {
      ptr = powTwo(i);
      strcpy(stepen, ptr);
      free(ptr);

      ptr = additionString(rez, stepen);
      strcpy(rez, ptr);
      free(ptr);
      // printf("%d - step - %s -> rez-> %s\n", i, stepen, rez);
    }
  }
  free(number);
  coup(rez);
  // printf("len - %d <> rez - %s \n", (int)strlen(rez), rez);
  char new_rez[MAX_NUM + 1] = "";
  if (mant < (int)strlen(rez)) {
    for (int i = 0; i < MAX_NUM; i++) {
      if (i == mant) {
        new_rez[i] = '.';
      } else if (i > mant)
        new_rez[i] = rez[i - 1];
      else
        new_rez[i] = rez[i];
    }
    coup(new_rez);
  } else if (mant == (int)strlen(rez)) {
    coup(new_rez);
    coup(rez);
    for (int i = mant - 1; i < MAX_NUM; i++) {
      if (i == mant)
        new_rez[i - mant + 1] = '.';
      else if (i > mant)
        new_rez[i - mant + 1] = rez[i - mant - 1];
      else
        new_rez[i - mant + 1] = '0';
    }
    coup(rez);
  } else {
    for (int i = 0; i < MAX_NUM; i++) {
      if (i >= (int)strlen(rez) && i < mant)
        new_rez[i] = '0';
      else if (i == mant)
        new_rez[i] = '.';
      else if (i > mant) {
        new_rez[i] = '0';
        break;  // added break - extra 0s before ','
      } else
        new_rez[i] = rez[i];
    }
    coup(new_rez);
  }

  printf("dec     -> %s \n", new_rez);
}

char* toBinary(int n) {
  int len = 32;
  char* binary = (char*)malloc(sizeof(char) * (len + 1));
  int k = 0;
  for (unsigned i = (1 << (len - 1)); i > 0; i = i / 2) {
    binary[k] = (n & i) ? '1' : '0';
    k++;
  }
  binary[k] = '\0';
  return binary;
}

char* additionString(char* a, char* b) {
  coup(a);
  coup(b);
  // printf("%s %s\n", a, b);
  int len_a = strlen(a);
  int len_b = strlen(b);
  int len_c = 0;

  if (len_a > len_b) {
    len_c = (len_a + 1);
    for (int j = len_b; j < len_c; j++) strcat(b, "0");
  } else {
    len_c = (len_b + 1);
    for (int j = len_a; j < len_c; j++) strcat(a, "0");
  }

  char* c = (char*)calloc(sizeof(char), MAX_NUM + 1);
  int flag = 0;
  int num = 0;
  for (int i = 0; i < len_c - 1; i++) {
    if (flag)
      flag = 0;
    else
      c[i] = '0';
    num = (a[i] - '0') + (b[i] - '0') + (c[i] - '0');

    if (num >= 10) {
      c[i + 1] = num / 10 + '0';
      // printf("%d %d\n", num/10, num%10);
      num = num % 10;
      flag = 1;
    }

    c[i] = num + '0';
    // printf("c - %c\n", c[i]);
  }
  coup(a);
  coup(b);
  // printf("%s \n", c);
  coup(c);

  return c;
}

void coup(char* tex) {
  int len = strlen(tex);
  char* new_tex = calloc(len + 1, 1);
  for (int i = 0; i < len; i++) {
    new_tex[i] = tex[len - i - 1];
  }
  for (int i = 0; i < len; i++) {
    tex[i] = new_tex[i];
  }
  free(new_tex);
}

char* powTwo(int b) {
  char two[2] = "2";
  char* rez = (char*)calloc(sizeof(char), MAX_NUM + 1);
  rez[0] = '2';
  char* ptr;
  if (b != 1) {
    for (int i = 0; i < b - 1; i++) {
      ptr = multiplicationString(rez, two);
      strcpy(rez, ptr);
      free(ptr);
    }
  }
  return rez;
}

char* multiplicationString(char* a, char* b) {
  int i_b = atoi(b);
  char* c = (char*)calloc(sizeof(char), MAX_NUM + 1);
  char* ptr = NULL;
  c[0] = '0';
  for (int i = 0; i < i_b; i++) {
    ptr = additionString(c, a);
    strcpy(c, ptr);
    // printf("c - %s\n", c);
    free(ptr);
  }
  return c;
}

int toDec(char* bin) {
  coup(bin);
  int len = strlen(bin);
  char rez[MAX_NUM + 1] = "";
  char* ptr = NULL;
  char stepen[MAX_NUM + 1];

  for (int i = 0; i < len; i++) {
    if (i == 0 && bin[i] == '1') {
      rez[0] = '1';
      continue;
    }
    if (bin[i] == '1') {
      ptr = powTwo(i);
      strcpy(stepen, ptr);
      free(ptr);

      ptr = additionString(rez, stepen);
      strcpy(rez, ptr);
      free(ptr);
      // printf("%d - step - %s -> rez-> %s\n", i, stepen, rez);
    }
  }
  // printf("scale   -> 10^%s \n", rez);
  return atoi(rez);
}