/**
 * Test Case 5: Modern OpenMP Features and Task Parallelism
 * Demonstrates: tasks, taskwait, depend clauses, simd
 * Complexity: Cutting Edge
 */
#include <stdio.h>
#include <stdlib.h>

// Fibonacci with tasks (demonstrating recursive task parallelism)
int fib_task(int n) {
    if (n < 2) return n;
    
    int x, y;
    #pragma omp task shared(x)
    x = fib_task(n - 1);
    
    #pragma omp task shared(y)
    y = fib_task(n - 2);
    
    #pragma omp taskwait
    return x + y;
}

int main() {
    const int N = 10000;
    int *data = (int*)malloc(N * sizeof(int));
    int *result = (int*)malloc(N * sizeof(int));
    
    // SIMD directive for vectorization
    #pragma omp simd
    for (int i = 0; i < N; i++) {
        data[i] = i * i;
    }
    
    // Task-based parallel processing
    #pragma omp parallel
    {
        #pragma omp single
        {
            for (int i = 0; i < 100; i++) {
                #pragma omp task firstprivate(i)
                {
                    // Simulate complex computation
                    int local_sum = 0;
                    for (int j = i * 100; j < (i + 1) * 100 && j < N; j++) {
                        local_sum += data[j];
                    }
                    result[i] = local_sum;
                    
                    if (i < 10) {
                        printf("Task %d completed with sum %d\n", i, local_sum);
                    }
                }
            }
        }
    }
    
    // Parallel for with SIMD and reduction
    int total_sum = 0;
    #pragma omp parallel for simd reduction(+:total_sum) schedule(simd:static)
    for (int i = 0; i < 100; i++) {
        total_sum += result[i];
    }
    
    // Demonstrate task dependencies (if supported)
    #pragma omp parallel
    {
        #pragma omp single
        {
            int fib_result;
            #pragma omp task shared(fib_result)
            {
                fib_result = fib_task(20);
            }
            
            #pragma omp taskwait
            printf("Fibonacci(20) = %d\n", fib_result);
        }
    }
    
    printf("Total sum: %d\n", total_sum);
    
    free(data);
    free(result);
    return 0;
}
