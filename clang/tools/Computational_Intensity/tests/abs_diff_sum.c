#include <math.h>
float abs_diff_sum(float* a, float* b, int N) {
    float sum = 0.0f;
    for (int i = 0; i < N; ++i) {
        sum += fabs(a[i] - b[i]);
    }
    return sum;
}
