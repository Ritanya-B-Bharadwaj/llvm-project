#include <math.h>
void power_series(float* a, int n) {
    for (int i = 0; i < n; ++i) {
        a[i] = exp(a[i]) - 1;
    }
}