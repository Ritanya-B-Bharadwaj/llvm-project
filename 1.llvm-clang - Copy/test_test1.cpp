// test_test1.cpp
#include <assert.h>
#include <stdio.h>

#define N 1000

// Replicate compute_section_sum from test1.cpp
int compute_section_sum(int* array, int start, int end) {
    int sum = 0;
    for (int i = start; i < end; ++i) {
        sum += array[i];
    }
    return sum;
}

void test_sequential_sum() {
    int array[N];
    double sum = 0.0;
    for (int i = 0; i < N; ++i) array[i] = i;
    for (int i = 0; i < N; ++i) sum += array[i];
    double expected = (N - 1) * N / 2.0;
    assert(sum == expected);
}

void test_section_sums() {
    int array[N];
    for (int i = 0; i < N; ++i) array[i] = i;
    int section1_sum = compute_section_sum(array, 0, N / 2);
    int section2_sum = compute_section_sum(array, N / 2, N);
    int expected1 = (N/2 - 1) * (N/2) / 2;
    int expected2 = ((N-1)*N/2) - expected1;
    assert(section1_sum == expected1);
    assert(section2_sum == expected2);
}

void test_elementwise_double() {
    int array[N];
    for (int i = 0; i < N; ++i) array[i] = i;
    for (int i = 0; i < N; ++i) array[i] *= 2;
    assert(array[0] == 0);
    assert(array[1] == 2);
    assert(array[N-1] == 2*(N-1));
}

void test_increment() {
    int array[N];
    for (int i = 0; i < N; ++i) array[i] = i * 2;
    for (int i = 0; i < N; ++i) array[i] += 1;
    assert(array[0] == 1);
    assert(array[1] == 3);
    assert(array[N-1] == 2*(N-1) + 1);
}

int main() {
    test_sequential_sum();
    test_section_sums();
    test_elementwise_double();
    test_increment();
    printf("PASS\n");
    return 0;
}