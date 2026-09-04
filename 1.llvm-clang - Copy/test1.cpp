#include <omp.h>
#include <stdio.h>

int main() {
    double sum = 0.0;
#pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < 100; ++i) {
        sum += 1.0 / (i + 1);
    }
    printf("harmonic sum=%.5f\n", sum);
    return 0;
}