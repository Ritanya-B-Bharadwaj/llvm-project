// filepath: omp_ir_mapper/examples/sample_omp.cpp
#include <omp.h>
#include <iostream>

int main() {
    int n = 10;
    int a[n], b[n], c[n];

    // Initialize arrays
    for (int i = 0; i < n; i++) {
        a[i] = i;
        b[i] = i * 2;
    }

    // Parallel region
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }

    // Print results
    for (int i = 0; i < n; i++) {
        std::cout << "c[" << i << "] = " << c[i] << std::endl;
    }

    return 0;
}