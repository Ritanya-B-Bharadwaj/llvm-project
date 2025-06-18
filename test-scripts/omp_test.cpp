#include <omp.h>
#include <iostream>

void foo() {
    int i;
    #pragma omp parallel for
    for (i = 0; i < 5; ++i) {
        std::cout << "Thread " << omp_get_thread_num() << " i = " << i << std::endl;
    }
}

int main() {
    foo();
    return 0;
}
