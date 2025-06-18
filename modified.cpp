#include <iostream>
#include <vector>
#include <chrono>

float compute(float x) {
    return x * __bf16(3.140625) + 2.7182818f - __bf16(1.4140625);
}

int main() {
    std::vector<float> data(10'000'000, __bf16(1));
    float sum = 0.0f;

    auto start = std::chrono::high_resolution_clock::now();
    for (auto &x : data)
        sum += compute(x);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = end - start;
    std::cout << "Result: " << sum << "\n";
    std::cout << "Elapsed Time: " << diff.count() << " s\n";
    return 0;
}
