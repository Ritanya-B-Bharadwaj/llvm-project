
void polynomial(float* a, int n) {
    for (int i = 0; i < n; ++i) {
        a[i] = a[i]*a[i]*a[i] + 3*a[i]*a[i] + 2*a[i] + 1;
    }
}
