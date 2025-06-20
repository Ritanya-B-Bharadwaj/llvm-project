#include <iostream>

// Function to add two numbers
int add(int x, int y) {
    return x + y;
}

int main() {
    int num1 = 10, num2 = 20;
    int sum = add(num1, num2);  // Call the function

    std::cout << "Sum: " << sum;
    return 0;
}