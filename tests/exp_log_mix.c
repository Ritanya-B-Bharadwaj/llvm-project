#include <math.h>
void exp_log_mix(float* x, float* y, int N) {
    for (int i = 0; i < N; ++i) {
        y[i] = log(x[i]) + exp(x[i]);
    }
}