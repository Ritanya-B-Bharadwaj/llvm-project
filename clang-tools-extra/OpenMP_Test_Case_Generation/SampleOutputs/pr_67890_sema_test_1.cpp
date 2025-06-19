
// RUN: %clang_cc1 -fopenmp -fsyntax-only -verify %s

void test1() {
  int x = 5;
  #pragma omp parallel for reduction(+:x)
  for (int i = 0; i < 10; i++) {
    x = x - 10; // expected-error {{invalid reduction operation for 'x'}}
  }
}

void test2() {
  unsigned int x = 5;
  #pragma omp parallel for reduction(+:x)
  for (int i = 0; i < 10; i++) {
    x = x - 10; // expected-error {{invalid reduction operation for 'x'}}
  }
}

void test3() {
  int x = 5;
  #pragma omp parallel for reduction(-:x)
  for (int i = 0; i < 10; i++) {
    x = x + 10; // expected-error {{invalid reduction operation for 'x'}}
  }
}

void test4() {
  unsigned int x = 5;
  #pragma omp parallel for reduction(-:x)
  for (int i = 0; i < 10; i++) {
    x = x + 10; // expected-error {{invalid reduction operation for 'x'}}
  }
}
