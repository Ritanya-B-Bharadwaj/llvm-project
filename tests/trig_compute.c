#include <math.h>
void trig_compute(float* a, int n) {
    for (int i = 0; i < n; ++i) {
        a[i] = sin(a[i]) + cos(a[i]);
    }
}