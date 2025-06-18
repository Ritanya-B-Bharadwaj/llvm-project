#include <omp.h>
#include <stdio.h>

int main() {
    int sum = 0;

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        printf("Hello from thread %d\n", tid);

        #pragma omp barrier

        #pragma omp for
        for (int i = 0; i < 10; ++i) {
            #pragma omp critical
            {
                sum += i;
            }
        }
    }

    printf("Sum is %d\n", sum);
    return 0;
}
