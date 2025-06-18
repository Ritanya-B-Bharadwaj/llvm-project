
void normalize(float* a, int n) {
    float sum = 0;
    for (int i = 0; i < n; ++i) sum += a[i];
    for (int i = 0; i < n; ++i) a[i] /= sum;
}