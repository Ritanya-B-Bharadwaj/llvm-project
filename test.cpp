#include <iostream>
#include <vector>
#include <chrono>

float compute(float x) {
    return x * 3.1415926f + 2.7182818f - 1.4142135f;
}

int main() {
    std::vector<float> data(10'000'000, 1.0f);
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
