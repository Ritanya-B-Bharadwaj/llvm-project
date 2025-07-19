int dce_test(int x) {
    int used = x * 2;
    int unused1 = x + 100;      // Dead - never used
    int unused2 = unused1 * 5;  // Dead - never used
    int unused3 = 42;           // Dead - never used
    
    return used;  // Only 'used' is returned
}