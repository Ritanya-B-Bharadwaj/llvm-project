// Example 3: Loop constructs
// This example demonstrates different types of loops and their LLVM IR

int loops_example(int n) {
    int sum = 0;
    
    // For loop
    for (int i = 0; i < n; i++) {
        sum += i;
    }
    
    // While loop
    int j = 0;
    while (j < n) {
        sum += j * 2;
        j++;
    }
    
    return sum;
}
