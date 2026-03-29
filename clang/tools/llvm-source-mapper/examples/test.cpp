// Test file for llvm-source-mapper
// This file contains various C++ constructs to test the mapping functionality

#include <iostream>

// Simple function
int add(int a, int b) {
    return a + b;
}

// Function with control flow
int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

// Function with loops
int factorial(int n) {
    int result = 1;
    for (int i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

// Main function with various operations
int main() {
    int x = 5;
    int y = 10;
    
    std::cout << "Sum: " << add(x, y) << std::endl;
    std::cout << "Max: " << max(x, y) << std::endl;
    std::cout << "Factorial: " << factorial(x) << std::endl;
    
    return 0;
}
