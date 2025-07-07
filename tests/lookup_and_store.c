void lookup_and_store(int* indices, float* input, float* output, int N) {
    for (int i = 0; i < N; ++i) {
        output[i] = input[indices[i]];
    }
}

