#include <math.h>

void heavy_compute(float *data, int n) {
    for (int i = 0; i < n; ++i) {
        float x = (float)i;
        float y = sin(x) + cos(x);
        float z = x * y + x / (y + 1.0f) - sqrtf(x + 2.0f);
        float w = exp(z) * z;
        data[0] = w;  // only a single memory store
    }
}
