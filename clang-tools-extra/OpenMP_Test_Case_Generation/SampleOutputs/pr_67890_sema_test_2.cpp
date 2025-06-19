
// RUN: %clang_cc1 -fopenmp -fsyntax-only -verify %s

void test1() {
  int i, x = 5;
#pragma omp parallel for reduction(+:x)
  for (i = 0; i < 10; i++) {
    x -= 2;
  }
}

void test2() {
  int i, x = 5;
#pragma omp parallel for reduction(-:x)
  for (i = 0; i < 10; i++) {
    x += 2;
  }
}

void test3() {
  int i, x = 5;
#pragma omp parallel for reduction(*:x)
  for (i = 0; i < 10; i++) {
    x /= 2;
  }
}

void test4() {
  int i, x = 5;
#pragma omp parallel for reduction(&&:x)
  for (i = 0; i < 10; i++) {
    x -= 2;
  }
}

void test5() {
  int i, x = 5;
#pragma omp parallel for reduction(||:x)
  for (i = 0; i < 10; i++) {
    x += 2;
  }
}

void test6() {
  int i, x = 5;
#pragma omp parallel for reduction(^:x)
  for (i = 0; i < 10; i++) {
    x /= 2;
  }
// expected-error{{invalid operator '^' in reduction clause}}
}

void test7() {
  int i, x = 5;
#pragma omp parallel for reduction(%:x)
  for (i = 0; i < 10; i++) {
    x -= 2;
  }
// expected-error{{invalid operator '%' in reduction clause}}
}

void test8() {
  int i, x = 5;
#pragma omp parallel for reduction(!:x)
  for (i = 0; i < 10; i++) {
    x += 2;
  }
// expected-error{{invalid operator '!' in reduction clause}}
}
