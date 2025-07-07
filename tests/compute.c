#include<math.h>
void compute(float* data, int N) {
    for (int i = 0; i < N; ++i) {
        data[i] = sin(data[i]) + cos(data[i]);
    }
}