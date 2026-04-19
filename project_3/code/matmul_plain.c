#include "matrix.h"

#include <stdio.h>

bool matmul_plain(const Matrix *a, const Matrix *b, Matrix *c) {
    /* Parameter validation */
    if (a == NULL || b == NULL || c == NULL) {
        fprintf(stderr, "Error: NULL matrix pointer in matmul_plain.\n");
        return false;
    }
    if (a->data == NULL || b->data == NULL || c->data == NULL) {
        fprintf(stderr, "Error: NULL data pointer in matmul_plain.\n");
        return false;
    }
    if (a->cols != b->rows) {
        fprintf(stderr, "Error: incompatible dimensions for multiplication "
                "(%zu x %zu) * (%zu x %zu).\n",
                a->rows, a->cols, b->rows, b->cols);
        return false;
    }
    if (c->rows != a->rows || c->cols != b->cols) {
        fprintf(stderr, "Error: output matrix dimensions mismatch. "
                "Expected (%zu x %zu), got (%zu x %zu).\n",
                a->rows, b->cols, c->rows, c->cols);
        return false;
    }

    size_t m = a->rows;  /* rows of A and C */
    size_t n = b->cols;  /* cols of B and C */
    size_t k = a->cols;  /* cols of A = rows of B */

    const float *data_a = a->data;
    const float *data_b = b->data;
    float *data_c = c->data;

    /* Zero out the result matrix */
    for (size_t i = 0; i < m * n; i++) {
        data_c[i] = 0.0f;
    }

    /*
     * Straightforward triple-nested loop: i, j, k order.
     * C[i][j] = sum over k of A[i][k] * B[k][j]
     *
     * This is the naive approach with no optimizations.
     * Access pattern for B is column-wise (stride = n), which is
     * cache-unfriendly. This serves as the performance baseline.
     */
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            float sum = 0.0f;
            for (size_t ki = 0; ki < k; ki++) {
                sum += data_a[i * k + ki] * data_b[ki * n + j];
            }
            data_c[i * n + j] = sum;
        }
    }

    return true;
}
