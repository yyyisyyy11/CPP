#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cblas.h>

/**
 * @brief Measure wall-clock time of a matrix multiplication function.
 *
 * @param func The matmul function to benchmark.
 * @param a Input matrix A.
 * @param b Input matrix B.
 * @param c Output matrix C.
 * @param label A descriptive label for printing results.
 * @return Elapsed time in seconds, or -1.0 on failure.
 */
static double benchmark(bool (*func)(const Matrix *, const Matrix *, Matrix *),
                        const Matrix *a, const Matrix *b, Matrix *c,
                        const char *label) {
    matrix_zero(c);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    bool ok = func(a, b, c);

    clock_gettime(CLOCK_MONOTONIC, &end);

    if (!ok) {
        fprintf(stderr, "Error: %s failed.\n", label);
        return -1.0;
    }

    double elapsed = (double)(end.tv_sec - start.tv_sec)
                   + (double)(end.tv_nsec - start.tv_nsec) / 1e9;

    /* Calculate GFLOPS: 2 * m * n * k floating-point operations */
    double flops = 2.0 * (double)a->rows * (double)b->cols * (double)a->cols;
    double gflops = flops / elapsed / 1e9;

    printf("  %-20s : %10.4f sec  |  %8.3f GFLOPS\n",
           label, elapsed, gflops);

    return elapsed;
}

/**
 * @brief Benchmark OpenBLAS cblas_sgemm as a reference implementation.
 *
 * Calls cblas_sgemm(CblasRowMajor, ...) to compute C = A * B,
 * measuring wall-clock time and reporting GFLOPS.
 *
 * @param a Input matrix A.
 * @param b Input matrix B.
 * @param c Output matrix C.
 * @param label Descriptive label for printing.
 * @return Elapsed time in seconds.
 */
static double benchmark_openblas(const Matrix *a, const Matrix *b, Matrix *c,
                                 const char *label) {
    matrix_zero(c);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    /* C = 1.0 * A * B + 0.0 * C */
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                (int)a->rows, (int)b->cols, (int)a->cols,
                1.0f,
                a->data, (int)a->cols,
                b->data, (int)b->cols,
                0.0f,
                c->data, (int)c->cols);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (double)(end.tv_sec - start.tv_sec)
                   + (double)(end.tv_nsec - start.tv_nsec) / 1e9;

    double flops = 2.0 * (double)a->rows * (double)b->cols * (double)a->cols;
    double gflops = flops / elapsed / 1e9;

    printf("  %-20s : %10.4f sec  |  %8.3f GFLOPS\n",
           label, elapsed, gflops);

    return elapsed;
}

/**
 * @brief Run matrix multiplication tests for a given size.
 *
 * @param n Matrix dimension (n x n).
 * @param run_plain Whether to run the plain (naive) benchmark.
 */
static void run_test(size_t n, bool run_plain) {
    printf("\n========================================\n");
    printf("  Matrix size: %zu x %zu\n", n, n);
    printf("========================================\n");

    Matrix *a = matrix_create(n, n);
    Matrix *b = matrix_create(n, n);
    Matrix *c_plain = matrix_create(n, n);
    Matrix *c_improved = matrix_create(n, n);
    Matrix *c_blas = matrix_create(n, n);

    if (a == NULL || b == NULL || c_plain == NULL ||
        c_improved == NULL || c_blas == NULL) {
        fprintf(stderr, "Error: failed to allocate %zu x %zu matrices.\n", n, n);
        matrix_free(a);
        matrix_free(b);
        matrix_free(c_plain);
        matrix_free(c_improved);
        matrix_free(c_blas);
        return;
    }

    /* Fill with random values */
    matrix_random(a);
    matrix_random(b);

    /* Benchmark matmul_plain */
    if (run_plain) {
        benchmark(matmul_plain, a, b, c_plain, "matmul_plain");
    }

    /* Benchmark matmul_improved */
    benchmark(matmul_improved, a, b, c_improved, "matmul_improved");

    /* Benchmark OpenBLAS */
    benchmark_openblas(a, b, c_blas, "OpenBLAS sgemm");

    /* Verify correctness: improved vs plain */
    if (run_plain) {
        float tol = 1e-4f;
        if (matrix_equal(c_plain, c_improved, tol)) {
            printf("  ✅ improved vs plain: match (tol = %.0e)\n", tol);
        } else {
            printf("  ❌ improved vs plain: MISMATCH!\n");
        }
    }

    /* Verify correctness: improved vs OpenBLAS */
    {
        float tol = 1e-3f;  /* slightly relaxed for BLAS accumulation order */
        if (matrix_equal(c_improved, c_blas, tol)) {
            printf("  ✅ improved vs OpenBLAS: match (tol = %.0e)\n", tol);
        } else {
            printf("  ❌ improved vs OpenBLAS: MISMATCH!\n");
        }
    }

    /* Clean up */
    matrix_free(a);
    matrix_free(b);
    matrix_free(c_plain);
    matrix_free(c_improved);
    matrix_free(c_blas);
}

int main(void) {
    printf("Matrix Multiplication Benchmark\n");
    printf("================================\n");

    srand((unsigned int)time(NULL));

    /* Small and medium sizes: run both plain and improved */
    size_t small_sizes[] = {16, 128, 1024};
    size_t num_small = sizeof(small_sizes) / sizeof(small_sizes[0]);

    for (size_t i = 0; i < num_small; i++) {
        run_test(small_sizes[i], true);
    }

    /* Large sizes */
    printf("\n--- Large matrices ---\n");
    run_test(8192, false);

    return 0;
}
