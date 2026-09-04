void scatter(float* data, float* output, int* indices, int N) {
  for (int i = 0; i < N; ++i) {
    output[indices[i]] = data[i];
  }
}