#include <cstdlib>
#include <ctime>

void compute(int* buffer, int size) {
    for (int i = 0; i < size; ++i) {
        buffer[i] = buffer[i] * buffer[i];  // read + write
    }
}

int main() {
    std::srand(std::time(nullptr));
    
    int* data1 = (int*)malloc(5 * sizeof(int));
    int* data2 = (int*)malloc(5 * sizeof(int));

    for (int i = 0; i < 5; ++i) {
        data1[i] = i + 1;
        data2[i] = std::rand() % 100;
    }

    compute(data1, 5);
    
    if (data2[2] % 2 == 0) {  // conditional read
        data2[2] = 42;        // write
    }

    free(data1);
    free(data2);
    return 0;
}

