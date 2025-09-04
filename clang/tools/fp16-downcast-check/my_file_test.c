#include <stdio.h>

int main() {
    // Exactly representable in both fp16 and bf16
    float a = 1.0f;
    float b = 2.0f;
    float c = 1.5f;

    // Within threshold, but not exact (will generate warning + note)
    float pi = 3.14159f;
    float e = 2.7182818f;
    double dsmall = 1.2345678;
    double dlarge = 65504.0; // Largest fp16 value, but bf16 will lose precision

    // Exceeds threshold (should generate note about exceeding threshold)
    float big = 1234567.0f;       // Too large for fp16, bf16 will round
    float tiny = 0.0001234f;      // Too small for fp16, bf16 will round to zero
    double dexceed = 1.00000123456789; // Too many digits: not representable within threshold

    // Negative values (same rules apply)
    float neg = -3.14159f;
    float neg_exceed = -1e-4f; // Too small for fp16

    printf("%f %f %f %f %f %f %f %f %f %f %f %f\n", a, b, c, pi, e, dsmall, dlarge, big, tiny, dexceed, neg, neg_exceed);
    return 0;
}