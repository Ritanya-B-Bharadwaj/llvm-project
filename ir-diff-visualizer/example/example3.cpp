int simplifycfg_test(int x) {
    if (true) {  // Always true - unnecessary branch
        if (x > 0) {
            return x + 1;
        } else {
            return x - 1;
        }
    }
    return 0;  // Unreachable
}