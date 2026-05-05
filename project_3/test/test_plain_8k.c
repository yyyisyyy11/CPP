#include "matrix.h"
#include <stdio.h>
#include <time.h>

int main(void) {
    size_t n = 8192;
    printf("Allocating 8192x8192 matrices...\n");
    Matrix *a = matrix_create(n, n);
    Matrix *b = matrix_create(n, n);
    Matrix *c = matrix_create(n, n);
    matrix_random(a); matrix_random(b); matrix_zero(c);

    printf("Running matmul_plain (8192x8192)... ~9 min\n");
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    matmul_plain(a, b, c);
    clock_gettime(CLOCK_MONOTONIC, &t1);

    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
    double gflops = 2.0 * n * n * n / elapsed / 1e9;
    printf("matmul_plain 8192x8192: %.2f sec | %.3f GFLOPS\n", elapsed, gflops);

    matrix_free(a); matrix_free(b); matrix_free(c);
    return 0;
}
