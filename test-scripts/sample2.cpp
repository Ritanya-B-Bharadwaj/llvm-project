#include <omp.h>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> data(100, 1);
    int totalSum = 0;

    // Outer parallel region
    #pragma omp parallel num_threads(4)
    {
        int tid = omp_get_thread_num();
        int nthreads = omp_get_num_threads();

        #pragma omp critical
        {
            std::cout << "[Outer] Thread " << tid << " of " << nthreads << " is active.\n";
        }

        // Parallel for loop
        #pragma omp for reduction(+:totalSum)
        for (int i = 0; i < data.size(); ++i) {
            totalSum += data[i];
        }

        // Sections
        #pragma omp sections
        {
            #pragma omp section
            {
                std::cout << "[Section 1] Thread " << omp_get_thread_num() << " running section 1\n";
            }

            #pragma omp section
            {
                std::cout << "[Section 2] Thread " << omp_get_thread_num() << " running section 2\n";
            }
        }

        // Nested parallel region
        #pragma omp parallel num_threads(2)
        {
            #pragma omp critical
            {
                std::cout << "[Nested] Thread " << omp_get_thread_num()
                          << " in nested region\n";
            }
        }
    }

    std::cout << "Total sum: " << totalSum << "\n";
    return 0;
}
