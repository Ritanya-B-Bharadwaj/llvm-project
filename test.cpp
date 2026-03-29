#include <iostream>

// A simple function that adds two integers
int add(int a, int b) {
    return a + b;
}

// A function with some control flow
int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

// A function with a loop
int sum(int n) {
    int result = 0;
    for (int i = 1; i <= n; i++) {
        result += i;
    }
    return result;
}

int main() {
    int x = 5;
    int y = 10;
    
    std::cout << "Sum: " << add(x, y) << std::endl;
    std::cout << "Max: " << max(x, y) << std::endl;
    std::cout << "Sum of 1 to 10: " << sum(10) << std::endl;
    
    return 0;
}