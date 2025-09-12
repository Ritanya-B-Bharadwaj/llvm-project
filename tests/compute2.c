#include<math.h>
void high_ci(float* data, int N) {
    for (int i = 0; i < N; ++i) {
        float x = data[i];
        float y = x * x + sin(x) * cos(x) + sqrt(x) + exp(x);
        float z = y * y + x;
        data[i] = z + y;
    }
}
