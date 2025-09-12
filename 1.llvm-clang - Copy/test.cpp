#include <omp.h>
#include <stdio.h>

int main() {
    int sum = 0;
#pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < 100; ++i) {
        sum += i;
    }
    printf("sum=%d\n", sum);
    return 0;
}
