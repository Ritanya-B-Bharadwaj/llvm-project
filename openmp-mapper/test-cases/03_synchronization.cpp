/**
 * Test Case 3: Synchronization and Data Environment
 * Demonstrates: critical, atomic, private, shared, firstprivate
 * Complexity: Advanced
 */
#include <stdio.h>

int shared_sum = 0;
int critical_sum = 0;

int main() {
    const int N = 1000;
    int data[N];
    int private_var = 10;
    
    // Initialize
    for (int i = 0; i < N; i++) {
        data[i] = i;
    }
    
    // Demonstrate different variable scoping
    #pragma omp parallel for private(private_var) shared(data)
    for (int i = 0; i < N; i++) {
        private_var = data[i] * 2;  // Each thread has its own copy
        data[i] = private_var;
    }
    
    // Atomic operations
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        #pragma omp atomic
        shared_sum += data[i];
    }
    
    // Critical section
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        #pragma omp critical
        {
            critical_sum += data[i];
            if (i % 100 == 0) {
                printf("Critical section: processing element %d\n", i);
            }
        }
    }
    
    // firstprivate demonstration
    int init_value = 42;
    #pragma omp parallel for firstprivate(init_value)
    for (int i = 0; i < 10; i++) {
        init_value += i;  // Each thread starts with 42
        printf("Thread processing %d: init_value = %d\n", i, init_value);
    }
    
    printf("Atomic sum: %d, Critical sum: %d\n", shared_sum, critical_sum);
    return 0;
}
