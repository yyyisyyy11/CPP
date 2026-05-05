/**
 * @file bench_size_sweep.c
 * @brief Size-sweep benchmark: measure improved vs OpenBLAS across many sizes.
 *
 * Purpose: Verify the hypothesis that the performance gap between our NEON
 * implementation and OpenBLAS (AMX) narrows as matrix size grows, because
 * small sizes are compute-bound (AMX advantage) while large sizes become
 * memory-bandwidth-bound (AMX advantage masked).
 *
 * Output: CSV-format data to stdout for easy plotting.
 */

#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cblas.h>

/**
 * @brief Benchmark matmul_improved, return GFLOPS.
 */
static double bench_improved(const Matrix *a, const Matrix *b, Matrix *c) {
    matrix_zero(c);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    matmul_improved(a, b, c);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (double)(end.tv_sec - start.tv_sec)
                   + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
    double flops = 2.0 * (double)a->rows * (double)b->cols * (double)a->cols;
    return flops / elapsed / 1e9;
}

/**
 * @brief Benchmark OpenBLAS sgemm, return GFLOPS.
 */
static double bench_openblas(const Matrix *a, const Matrix *b, Matrix *c) {
    matrix_zero(c);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

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
    return flops / elapsed / 1e9;
}

int main(void) {
    srand((unsigned int)time(NULL));

    /*
     * Test sizes chosen to span:
     *   - Fully in L1 (128²: 192 KB for 3 matrices)
     *   - Fits in L2 (512²: 3 MB)
     *   - Partially in L2 (1024²: 12 MB)
     *   - Exceeds all caches (2048+: 48+ MB)
     *
     * We include intermediate sizes to see the transition clearly.
     */
    size_t sizes[] = {128, 256, 384, 512, 640, 768, 896, 1024,
                      1280, 1536, 2048, 3072, 4096, 6144, 8192};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    /* Number of repetitions for small sizes (more reps → more stable) */
    int warmup_runs = 1;

    /* Print CSV header */
    printf("# Size-sweep benchmark: improved (NEON) vs OpenBLAS (AMX)\n");
    printf("# OMP_NUM_THREADS = %s\n",
           getenv("OMP_NUM_THREADS") ? getenv("OMP_NUM_THREADS") : "default");
    printf("size,data_MB,improved_gflops,openblas_gflops,gap_ratio\n");
    fflush(stdout);

    for (size_t i = 0; i < num_sizes; i++) {
        size_t n = sizes[i];
        double data_mb = 3.0 * n * n * 4.0 / (1024.0 * 1024.0);

        Matrix *a = matrix_create(n, n);
        Matrix *b = matrix_create(n, n);
        Matrix *c1 = matrix_create(n, n);
        Matrix *c2 = matrix_create(n, n);

        if (!a || !b || !c1 || !c2) {
            fprintf(stderr, "Error: failed to allocate %zu x %zu matrices.\n", n, n);
            matrix_free(a); matrix_free(b); matrix_free(c1); matrix_free(c2);
            continue;
        }

        matrix_random(a);
        matrix_random(b);

        /* Warmup run (first run may be slow due to thread creation, etc.) */
        for (int w = 0; w < warmup_runs; w++) {
            bench_improved(a, b, c1);
            bench_openblas(a, b, c2);
        }

        /* Measured run */
        double gf_improved = bench_improved(a, b, c1);
        double gf_openblas = bench_openblas(a, b, c2);
        double gap = gf_openblas / gf_improved;

        printf("%zu,%.1f,%.1f,%.1f,%.2f\n",
               n, data_mb, gf_improved, gf_openblas, gap);
        fflush(stdout);

        fprintf(stderr, "  [%2zu/%zu] %5zu: improved=%.1f  openblas=%.1f  gap=%.2fx\n",
                i + 1, num_sizes, n, gf_improved, gf_openblas, gap);

        matrix_free(a);
        matrix_free(b);
        matrix_free(c1);
        matrix_free(c2);
    }

    fprintf(stderr, "\nDone! Pipe stdout to a .csv file for plotting.\n");
    return 0;
}
