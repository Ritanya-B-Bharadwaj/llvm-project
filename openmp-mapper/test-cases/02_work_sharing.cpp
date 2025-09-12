/**
 * Test Case 2: Work Sharing Constructs
 * Demonstrates: sections, single, master, nowait, barrier
 * Complexity: Intermediate
 */
#include <stdio.h>

int global_counter = 0;

int main() {
    int data[1000];
    int sum1 = 0, sum2 = 0, sum3 = 0;
    
    // Initialize data
    #pragma omp parallel for
    for (int i = 0; i < 1000; i++) {
        data[i] = i;
    }
    
    // Parallel sections with different work distributions
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            for (int i = 0; i < 333; i++) {
                sum1 += data[i];
            }
            printf("Section 1 completed\n");
        }
        
        #pragma omp section
        {
            for (int i = 333; i < 666; i++) {
                sum2 += data[i];
            }
            printf("Section 2 completed\n");
        }
        
        #pragma omp section
        {
            for (int i = 666; i < 1000; i++) {
                sum3 += data[i];
            }
            printf("Section 3 completed\n");
        }
    }
    
    // Demonstrate single and master directives
    #pragma omp parallel
    {
        #pragma omp for nowait
        for (int i = 0; i < 100; i++) {
            data[i] *= 2;
        }
        
        #pragma omp single
        {
            printf("Single directive executed once\n");
            global_counter = sum1 + sum2 + sum3;
        }
        
        #pragma omp barrier
        
        #pragma omp master
        {
            printf("Master thread reporting: counter = %d\n", global_counter);
        }
    }
    
    return 0;
}
