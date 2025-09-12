/**
 * Test Case 1: Basic Parallel Constructs
 * Demonstrates: parallel for, reduction, basic scheduling
 * Complexity: Beginner
 */
#include <stdio.h>

int main() {
    const int N = 1000;
    int data[N];
    int sum = 0;
    
    // Basic parallel for loop
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        data[i] = i * 2;
    }
    
    // Parallel for with reduction
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < N; i++) {
        sum += data[i];
    }
    
    // Parallel for with scheduling
    #pragma omp parallel for schedule(static, 100)
    for (int i = 0; i < N; i++) {
        data[i] = data[i] + 1;
    }
    
    printf("Basic parallel test completed. Sum: %d\n", sum);
    return 0;
}
