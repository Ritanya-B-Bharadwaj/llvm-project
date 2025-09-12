#include <stdio.h>

int main() {
    // Exact downcast
    float x = 1.5f;

    // Within threshold example (pi)
    float pi = 3.14159f;

    // Outside threshold example (e)
    float e = 2.7182818f;

    // A double literal to check behaviour
    double big = 1.2345678945678;

    // Suppress unused-variable warnings
    printf("%f %f %f %f\n", x, pi, e, big);
    return 0;
} 