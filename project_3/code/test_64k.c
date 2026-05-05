/**
 * @file test_64k.c
 * @brief Memory-efficient 64K×64K benchmark.
 *
 * Only allocates 3 matrices (A, B, C) = 48 GB total.
 * Reuses C buffer for both improved and OpenBLAS runs.
 * Compares results via sampling (cannot afford a 4th 16 GB matrix).
 */
#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <cblas.h>

#define N 65536
#define NUM_SAMPLES 1000

int main(void) {
    printf("=== 64K×64K Matrix Multiplication Benchmark ===\n\n");

    size_t total_mem = (size_t)3 * N * N * sizeof(float);
    printf("Allocating 3 matrices: %.1f GB total\n", total_mem / 1e9);

    /* Allocate matrices */
    Matrix *a = matrix_create(N, N);
    Matrix *b = matrix_create(N, N);
    Matrix *c = matrix_create(N, N);

    if (a == NULL || b == NULL || c == NULL) {
        fprintf(stderr, "Error: failed to allocate %d x %d matrices.\n"
                "Need ~%.0f GB RAM. Check available memory.\n",
                N, N, total_mem / 1e9);
        matrix_free(a);
        matrix_free(b);
        matrix_free(c);
        return 1;
    }
    printf("Allocation OK.\n\n");

    /* Fill with random values */
    srand(42);  /* fixed seed for reproducibility */
    matrix_random(a);
    matrix_random(b);

    /* ── Run 1: matmul_improved ── */
    printf("Running matmul_improved...\n");
    matrix_zero(c);

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    bool ok = matmul_improved(a, b, c);
    clock_gettime(CLOCK_MONOTONIC, &t1);

    if (!ok) {
        fprintf(stderr, "matmul_improved failed!\n");
        matrix_free(a); matrix_free(b); matrix_free(c);
        return 1;
    }

    double elapsed_imp = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
    double flops = 2.0 * (double)N * (double)N * (double)N;
    double gflops_imp = flops / elapsed_imp / 1e9;

    printf("  matmul_improved : %10.2f sec  |  %8.3f GFLOPS\n\n", elapsed_imp, gflops_imp);

    /* Save sample values from improved result for comparison */
    srand(123);
    float samples_imp[NUM_SAMPLES];
    size_t sample_idx[NUM_SAMPLES];
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sample_idx[i] = (size_t)rand() % ((size_t)N * N);
        samples_imp[i] = c->data[sample_idx[i]];
    }

    /* ── Run 2: OpenBLAS ── */
    printf("Running OpenBLAS cblas_sgemm...\n");
    matrix_zero(c);

    clock_gettime(CLOCK_MONOTONIC, &t0);
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                N, N, N,
                1.0f, a->data, N, b->data, N,
                0.0f, c->data, N);
    clock_gettime(CLOCK_MONOTONIC, &t1);

    double elapsed_blas = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
    double gflops_blas = flops / elapsed_blas / 1e9;

    printf("  OpenBLAS sgemm  : %10.2f sec  |  %8.3f GFLOPS\n\n", elapsed_blas, gflops_blas);

    /* ── Compare results via sampling ── */
    int match = 0, mismatch = 0;
    float max_err = 0.0f;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        float diff = fabsf(samples_imp[i] - c->data[sample_idx[i]]);
        float scale = fmaxf(fabsf(samples_imp[i]), fabsf(c->data[sample_idx[i]]));
        float rel_err = (scale > 0) ? diff / scale : diff;
        if (rel_err > max_err) max_err = rel_err;
        if (rel_err < 1e-3f) match++;
        else mismatch++;
    }

    printf("Correctness check (%d random samples):\n", NUM_SAMPLES);
    if (mismatch == 0) {
        printf("  ✅ All samples match (max relative error = %.2e)\n", max_err);
    } else {
        printf("  ⚠️  %d/%d samples differ (max relative error = %.2e)\n",
               mismatch, NUM_SAMPLES, max_err);
    }

    printf("\n=== Summary ===\n");
    printf("  Matrix size     : %d × %d\n", N, N);
    printf("  improved        : %.2f sec → %.3f GFLOPS\n", elapsed_imp, gflops_imp);
    printf("  OpenBLAS        : %.2f sec → %.3f GFLOPS\n", elapsed_blas, gflops_blas);
    printf("  improved/BLAS   : %.1f%%\n", gflops_imp / gflops_blas * 100);

    matrix_free(a);
    matrix_free(b);
    matrix_free(c);
    return 0;
}
