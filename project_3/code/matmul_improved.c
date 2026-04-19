#include "matrix.h"

#include <stdio.h>
#include <string.h>

/*
 * Tile size for cache blocking.
 *
 * A tile of 64x64 floats = 64 * 64 * 4 = 16 KB.
 * Three tiles (A, B, C sub-blocks) = ~48 KB, which fits
 * comfortably in the Apple M5's 128 KB L1 data cache.
 */
#define TILE_SIZE 64

/**
 * @brief Compute a tile contribution: C_tile += A_tile * B_tile.
 *
 * Uses the cache-friendly i, k, j loop order so that both B and C
 * are accessed with stride-1 (contiguous) in the innermost loop.
 *
 * @param A     Pointer to element A[i_start][k_start] in the full matrix.
 * @param B     Pointer to element B[k_start][j_start] in the full matrix.
 * @param C     Pointer to element C[i_start][j_start] in the full matrix.
 * @param lda   Leading dimension (number of columns) of the full A matrix.
 * @param ldb   Leading dimension (number of columns) of the full B matrix.
 * @param ldc   Leading dimension (number of columns) of the full C matrix.
 * @param tile_m Number of rows in this tile (may be < TILE_SIZE at edges).
 * @param tile_n Number of columns in this tile.
 * @param tile_k Shared dimension of this tile.
 */
static inline void tile_multiply(const float *A, const float *B, float *C,
                                 size_t lda, size_t ldb, size_t ldc,
                                 size_t tile_m, size_t tile_n, size_t tile_k) {
    for (size_t i = 0; i < tile_m; i++) {
        for (size_t ki = 0; ki < tile_k; ki++) {
            /*
             * A[i][ki] is a scalar for this inner loop iteration.
             * The compiler will keep it in a register.
             */
            float a_ik = A[i * lda + ki];

            /*
             * Inner loop: j varies, so both B[ki][j] and C[i][j]
             * are accessed sequentially (stride-1) → cache friendly.
             */
            for (size_t j = 0; j < tile_n; j++) {
                C[i * ldc + j] += a_ik * B[ki * ldb + j];
            }
        }
    }
}

bool matmul_improved(const Matrix *a, const Matrix *b, Matrix *c) {
    /* Parameter validation */
    if (a == NULL || b == NULL || c == NULL) {
        fprintf(stderr, "Error: NULL matrix pointer in matmul_improved.\n");
        return false;
    }
    if (a->data == NULL || b->data == NULL || c->data == NULL) {
        fprintf(stderr, "Error: NULL data pointer in matmul_improved.\n");
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

    size_t m = a->rows;
    size_t n = b->cols;
    size_t k = a->cols;

    const float *data_a = a->data;
    const float *data_b = b->data;
    float *data_c = c->data;

    /* Zero out the result matrix */
    memset(data_c, 0, m * n * sizeof(float));

    /*
     * Tiled matrix multiplication with i, k, j loop order.
     *
     * Outer three loops iterate over tiles (blocks).
     * The order ii → kk → jj means:
     *   - For a fixed row-block of A (ii), we sweep through
     *     all k-blocks and j-blocks, accumulating into C.
     *   - This maximizes reuse of the A tile in registers/L1.
     */
    for (size_t ii = 0; ii < m; ii += TILE_SIZE) {
        size_t tile_m = (ii + TILE_SIZE <= m) ? TILE_SIZE : (m - ii);

        for (size_t kk = 0; kk < k; kk += TILE_SIZE) {
            size_t tile_k = (kk + TILE_SIZE <= k) ? TILE_SIZE : (k - kk);

            for (size_t jj = 0; jj < n; jj += TILE_SIZE) {
                size_t tile_n = (jj + TILE_SIZE <= n) ? TILE_SIZE : (n - jj);

                /*
                 * Compute C[ii..ii+tile_m][jj..jj+tile_n] +=
                 *     A[ii..ii+tile_m][kk..kk+tile_k] *
                 *     B[kk..kk+tile_k][jj..jj+tile_n]
                 */
                tile_multiply(
                    &data_a[ii * k + kk],     /* A sub-block start */
                    &data_b[kk * n + jj],     /* B sub-block start */
                    &data_c[ii * n + jj],     /* C sub-block start */
                    k, n, n,                  /* leading dimensions */
                    tile_m, tile_n, tile_k    /* actual tile sizes */
                );
            }
        }
    }

    return true;
}
