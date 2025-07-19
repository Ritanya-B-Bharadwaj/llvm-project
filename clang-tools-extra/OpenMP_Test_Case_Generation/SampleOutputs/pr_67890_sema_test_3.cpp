
// RUN: %clang_cc1 -fopenmp -fsyntax-only -verify %s

void test1() {
  int i, x = 5;
  #pragma omp parallel for
  for (i = 0; i < x; i++) {
    int j = i + 2147483647; // expected-error{{overflow in constant expression}}
  }
}

void test2() {
  int i, x = 5;
  #pragma omp parallel for
  for (i = 0; i < x; i++) {
    unsigned int j = i + 4294967295; // expected-error{{overflow in constant expression}}
  }
}

void test3() {
  int i, x = 5;
  #pragma omp parallel for
  for (i = 0; i < x; i++) {
    int j = i - 2147483648}}; // expected-error{{overflow in constant expression}}
  }
}

void test4() {
  int i, x = 5;
  #pragma omp parallel for
  for (i = 0; i < x; i++) {
    unsigned int j = i - 4294967296; // expected-error{{overflow in constant expression}}
  }
}
