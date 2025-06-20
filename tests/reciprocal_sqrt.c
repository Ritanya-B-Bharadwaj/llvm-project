
#include <math.h>
void reciprocal_sqrt(float* input, float* output, int N) {
    for (int i = 0; i < N; ++i) {
        output[i] = 1.0f / sqrt(input[i]);
    }
}