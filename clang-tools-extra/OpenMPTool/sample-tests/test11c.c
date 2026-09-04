void foo2(int *a, int n) {
  #pragma omp parallel for
  for (int i = 0; i < n; ++i) {
    a[i] = i;
  }
}

