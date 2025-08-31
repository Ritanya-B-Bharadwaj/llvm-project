// Example 4: Arrays and memory operations
// This example shows array operations and memory management

void array_operations() {
    int arr[5] = {1, 2, 3, 4, 5};
    int sum = 0;
    
    // Array access and modification
    for (int i = 0; i < 5; i++) {
        sum += arr[i];
        arr[i] = arr[i] * 2;
    }
    
    // Pointer operations
    int* ptr = arr;
    *ptr = 100;
    *(ptr + 1) = 200;
}
