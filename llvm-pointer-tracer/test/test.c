#include <stdio.h>
#include <stdlib.h>

void foo() {
    int x = 42;
    int y = 24;
    int *ptr1 = &x;
    int *ptr2 = &y;
    
    // These will generate LoadInst instructions
    int val1 = *ptr1;
    int val2 = *ptr2;
    
    // This will generate StoreInst instructions
    *ptr1 = val2;
    *ptr2 = val1;
}

void bar() {
    int arr[5] = {1, 2, 3, 4, 5};
    int *ptr = arr;
    
    // Array access - generates GEP and Load instructions
    for (int i = 0; i < 3; i++) {
        int val = ptr[i];  // Load from ptr + i
        ptr[i] = val * 2;  // Store to ptr + i
    }
}

int main() {
    foo();
    bar();
    return 0;
}
