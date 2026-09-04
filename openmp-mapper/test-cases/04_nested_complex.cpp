/**
 * Test Case 4: Nested Parallelism and Complex Scheduling
 * Demonstrates: nested parallel regions, dynamic/guided scheduling, collapse
 * Complexity: Expert
 */
#include <stdio.h>

int main() {
    const int ROWS = 100;
    const int COLS = 100;
    int matrix[ROWS][COLS];
    int vector[COLS];
    int result[ROWS];
    
    // Initialize matrix and vector
    #pragma omp parallel for collapse(2) schedule(dynamic, 10)
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            matrix[i][j] = i + j;
        }
    }
    
    #pragma omp parallel for schedule(guided)
    for (int j = 0; j < COLS; j++) {
        vector[j] = j * 2;
    }
    
    // Matrix-vector multiplication with nested parallelism
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < ROWS; i++) {
        result[i] = 0;
        
        // Inner parallel region (nested)
        #pragma omp parallel for reduction(+:result[i]) if(COLS > 50)
        for (int j = 0; j < COLS; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
    
    // Complex nested structure with multiple synchronization points
    #pragma omp parallel
    {
        #pragma omp for schedule(runtime) nowait
        for (int i = 0; i < ROWS; i++) {
            result[i] = result[i] / COLS;
        }
        
        #pragma omp barrier
        
        #pragma omp single nowait
        {
            printf("Single thread computing final statistics\n");
        }
        
        #pragma omp for schedule(static, 1)
        for (int i = 0; i < 10; i++) {
            printf("Final result[%d] = %d\n", i, result[i]);
        }
    }
    
    return 0;
}
