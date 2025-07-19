int fibonacci_calculator(int n) {
    int a = 0;      // Will be promoted to virtual register
    int b = 1;      // Will be promoted to virtual register
    int temp;       // Will be promoted to virtual register
    int result;     // Will be promoted to virtual register
    
    if (n <= 0) {
        result = 0;
    } else if (n == 1) {
        result = 1;
    } else {
        for (int i = 2; i <= n; i++) {
            temp = a + b;
            a = b;
            b = temp;
        }
        result = b;
    }
    
    return result;
}

int main() {
    int num = 10;
    int fib_result = fibonacci_calculator(num);
    return fib_result;
}