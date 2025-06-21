int instcombine_test(int x) {
    int step1 = x + 0;        // Add zero (identity)
    int step2 = step1 * 1;    // Multiply by one (identity)
    int step3 = step2 - 0;    // Subtract zero (identity)
    int step4 = step3 << 0;   // Shift by zero (identity)
    int step5 = step4 | 0;    // OR with zero (identity)
    
    return step5;
}