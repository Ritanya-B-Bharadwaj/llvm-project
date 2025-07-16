#include <iostream>

void greet() {
    std::cout << "Hello from greet!" << std::endl;
}

int add(int a, int b) {
    return a + b;
}

int main() {
    greet();
    int sum = add(3, 4);
    std::cout << "Sum: " << sum << std::endl;
    return 0;
}
