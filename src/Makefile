CFLAGS = -Wall -Wextra -Werror -pedantic -std=c11
TEST_FLAGS = -lm -lpthread -lrt
S21_SUITE_C = tests/*.c
S21_SUITE_O = suite_*.o
GCOV_FLAGS = -ftest-coverage -fprofile-arcs --coverage


HEADER = s21_decimal.h 
SOURCES_ALL = $(wildcard *.c)

ifeq ($(shell uname -s), Linux)	
	TEST_FLAGS = -lsubunit -lm -lpthread -lrt
endif

OBJ_LIBRARY := $(patsubst %.c, %.o, $(SOURCES_ALL))

%.o: %.c $(HEADER)
	gcc $(CFLAGS) -c $< -o $@

all: gcov_report

s21_decimal.a: $(OBJ_LIBRARY)  $(HEADER)
	ar rcs s21_decimal.a $(OBJ_LIBRARY)
	ranlib s21_decimal.a

test: s21_decimal.a
	gcc $(CFLAGS) -c $(S21_SUITE_C) tests/main.c
	gcc $(CFLAGS) -o test $(S21_SUITE_O) main.o s21_decimal.a -lcheck -lm -lsubunit	
	rm -rf *.o *.a
	./test




gcov_report: clean
	gcc $(SOURCES_ALL) $(S21_SUITE_C) -lcheck -o gcov_report --coverage $(TEST_FLAGS)
	./gcov_report
	rm -rf *suite_*.gc*
	rm -rf *main*.gc*
	lcov -d . -o gcov_report.info -c
	genhtml -o report gcov_report.info

check:
	clang-format -style=Google -i *.c *.h
	clang-format -style=Google -n *.c *.h
	clang-format -style=Google -i tests/*.c
	clang-format -style=Google -n tests/*.c

rebuild: clean all

clean:
	rm -f *.gcno *.gcda *.info *.gcov 
	rm -f *.o *.a
	rm -rf report
	rm -f *.out

