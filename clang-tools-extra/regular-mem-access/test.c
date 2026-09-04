#include <math.h>

// Regular memory access pattern
void compute(float* data, int N) {
    for (int i = 0; i < N; ++i) {
        data[i] = sin(data[i]) + cos(data[i]);
    }
}

// Irregular memory access pattern
void scatter(float* data, float* output, int* indices, int N) {
    for (int i = 0; i < N; ++i) {
        output[indices[i]] = data[i];
    }
}

// Regular: linear stride
void stride_access(float* data, int N) {
    for (int i = 0; i < N; ++i) {
        data[2*i+1] = data[2*i] + 1.0f;
    }
}

// Irregular: data-dependent index
void gather(float* data, float* output, int* indices, int N) {
    for (int i = 0; i < N; ++i) {
        output[i] = data[indices[i]];
    }
}
