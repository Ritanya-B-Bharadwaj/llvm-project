#include <iostream>
#include <vector>
#include <omp.h>
#include <stdio.h>

int main() {
    const int N = 1000;
    std::vector<int> data(N, 1);
    int sum = 0;

    // Parallel for with reduction
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < N; ++i) {
        sum += data[i];
    }

    std::cout << "Sum (parallel for with reduction): " << sum << std::endl;

    // Parallel sections
    int x = 0, y = 0;
    #pragma omp parallel sections
    {
        #pragma omp section
        x = 42;

        #pragma omp section
        y = 24;
    }
    std::cout << "x: " << x << ", y: " << y << std::endl;

    // Parallel for with critical section
    int counter = 0;
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        #pragma omp critical
        counter += 1;
    }
    std::cout << "Counter (with critical): " << counter << std::endl;

    // Parallel for with atomic
    int atomic_counter = 0;
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        #pragma omp atomic
        atomic_counter++;
    }
    std::cout << "Atomic Counter: " << atomic_counter << std::endl;

    // SIMD directive
    #pragma omp simd
    for (int i = 0; i < N; ++i) {
        data[i] *= 2;
    }

    std::cout << "Vector doubled. First element: " << data[0] << std::endl;

    return 0;
}
