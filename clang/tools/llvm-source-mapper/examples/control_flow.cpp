// Example 2: Control flow with if-else statements
// This example shows how conditional statements translate to LLVM IR

int control_flow(int x) {
    int result;
    
    if (x > 0) {
        result = x * 2;
    } else if (x < 0) {
        result = x * -1;
    } else {
        result = 1;
    }
    
    return result;
}
