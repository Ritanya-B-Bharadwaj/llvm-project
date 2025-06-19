#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1000

int main() {
    int *arr = (int*)malloc(N * sizeof(int));
    for (int i = 0; i < N; ++i) arr[i] = i + 1;

    long long sum = 0;
    long long product = 1;
    int max_val = 0;
    int even_count = 0;

    // Parallel region with multiple tasks
#pragma omp parallel
    {
        // Sum reduction
#pragma omp for reduction(+:sum)
        for (int i = 0; i < N; ++i) {
            sum += arr[i];
        }

        // Product reduction (to avoid overflow, use first 20 elements)
#pragma omp for reduction(*:product)
        for (int i = 0; i < 20; ++i) {
            product *= arr[i];
        }

        // Find maximum using critical section
#pragma omp for
        for (int i = 0; i < N; ++i) {
#pragma omp critical
            {
                if (arr[i] > max_val) max_val = arr[i];
            }
        }

        // Count even numbers using atomic
#pragma omp for
        for (int i = 0; i < N; ++i) {
            if (arr[i] % 2 == 0) {
#pragma omp atomic
                even_count++;
            }
        }
    }

    printf("Sum = %lld\n", sum);
    printf("Product of first 20 elements = %lld\n", product);
    printf("Max value = %d\n", max_val);
    printf("Even count = %d\n", even_count);

    free(arr);
    return 0;
}