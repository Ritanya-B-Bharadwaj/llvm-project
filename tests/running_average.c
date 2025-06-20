
void running_average(float* x, float* y, int N) {
    float sum = 0.0f;
    for (int i = 0; i < N; ++i) {
        sum += x[i];
        y[i] = sum / (i + 1);
    }
}
