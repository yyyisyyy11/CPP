#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * Memory alignment for matrix data allocation.
 *
 * Set to 128 bytes to match the Apple M5's cache line size
 * (verified via sysctl hw.cachelinesize = 128).
 * This ensures each row's starting address doesn't straddle
 * a cache line boundary, and prepares for SIMD aligned loads.
 */
#define MATRIX_ALIGN 128

/**
 * @brief Matrix structure for storing a 2D matrix of floats.
 *
 * Data is stored in row-major order: element at (row, col) is
 * accessed via data[row * cols + col].
 */
typedef struct {
    size_t rows;
    size_t cols;
    float *data;
} Matrix;

/* ── matmul declarations (defined in .c files) ── */

bool matmul_plain(const Matrix *a, const Matrix *b, Matrix *c);
bool matmul_improved(const Matrix *a, const Matrix *b, Matrix *c);

/* ══════════════════════════════════════════════════════════════════
 *  Implementation Section (single-header library)
 *
 *  All utility functions are defined here so that only matrix.h
 *  and two .c files need to be submitted.
 * ══════════════════════════════════════════════════════════════════ */

/**
 * @brief Create a matrix with the given dimensions.
 *
 * Allocates memory for the matrix data (initialized to zero).
 * Returns NULL if allocation fails or dimensions are zero.
 *
 * @param rows Number of rows.
 * @param cols Number of columns.
 * @return Pointer to the newly created Matrix, or NULL on failure.
 */
static inline Matrix *matrix_create(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0) {
        fprintf(stderr, "Error: matrix dimensions must be non-zero.\n");
        return NULL;
    }
    if (rows > 65536 || cols > 65536) {
        fprintf(stderr, "Error: matrix dimensions too large "
                "(%zu x %zu, max 65536 x 65536).\n", rows, cols);
        return NULL;
    }

    Matrix *mat = (Matrix *)malloc(sizeof(Matrix));
    if (mat == NULL) {
        fprintf(stderr, "Error: failed to allocate Matrix struct.\n");
        return NULL;
    }

    mat->rows = rows;
    mat->cols = cols;

    /*
     * Use posix_memalign for cache-line-aligned allocation.
     * This avoids cache line splits and prepares for SIMD aligned loads.
     * posix_memalign works with free() and has no size-multiple constraint.
     */
    if (rows > SIZE_MAX / cols ||
        rows * cols > SIZE_MAX / sizeof(float)) {
        fprintf(stderr, "Error: matrix size overflow "
                "(%zu x %zu exceeds SIZE_MAX).\n", rows, cols);
        free(mat);
        return NULL;
    }
    size_t data_size = rows * cols * sizeof(float);
    int ret = posix_memalign((void **)&mat->data, MATRIX_ALIGN, data_size);
    if (ret != 0 || mat->data == NULL) {
        fprintf(stderr, "Error: failed to allocate aligned matrix data "
                "(%zu x %zu, align=%d).\n", rows, cols, MATRIX_ALIGN);
        free(mat);
        return NULL;
    }
    memset(mat->data, 0, data_size);

    return mat;
}

/**
 * @brief Free the memory associated with a matrix.
 *
 * Safely handles NULL pointers.
 *
 * @param mat Pointer to the matrix to free.
 */
static inline void matrix_free(Matrix *mat) {
    if (mat != NULL) {
        free(mat->data);
        mat->data = NULL;
        mat->rows = 0;
        mat->cols = 0;
        free(mat);
    }
}

/**
 * @brief Fill a matrix with random float values in [0, 1).
 *
 * @param mat Pointer to the matrix to fill.
 */
static inline void matrix_random(Matrix *mat) {
    if (mat == NULL || mat->data == NULL) {
        fprintf(stderr, "Error: cannot fill NULL matrix with random values.\n");
        return;
    }

    size_t total = mat->rows * mat->cols;
    for (size_t i = 0; i < total; i++) {
        mat->data[i] = (float)rand() / (float)RAND_MAX;
    }
}

/**
 * @brief Fill a matrix with zeros.
 *
 * @param mat Pointer to the matrix to fill.
 */
static inline void matrix_zero(Matrix *mat) {
    if (mat == NULL || mat->data == NULL) {
        fprintf(stderr, "Error: cannot zero NULL matrix.\n");
        return;
    }

    memset(mat->data, 0, mat->rows * mat->cols * sizeof(float));
}

/**
 * @brief Check if two matrices are approximately equal.
 *
 * Compares element-wise with a relative and absolute tolerance.
 *
 * @param a First matrix.
 * @param b Second matrix.
 * @param tolerance Maximum allowed absolute difference per element.
 * @return true if all elements are within tolerance, false otherwise.
 */
static inline bool matrix_equal(const Matrix *a, const Matrix *b, float tolerance) {
    if (a == NULL || b == NULL) {
        fprintf(stderr, "Error: cannot compare NULL matrices.\n");
        return false;
    }
    if (a->rows != b->rows || a->cols != b->cols) {
        fprintf(stderr, "Error: matrix dimensions do not match "
                "(%zu x %zu vs %zu x %zu).\n",
                a->rows, a->cols, b->rows, b->cols);
        return false;
    }

    size_t total = a->rows * a->cols;
    for (size_t i = 0; i < total; i++) {
        float diff = fabsf(a->data[i] - b->data[i]);
        /* Use both absolute and relative tolerance */
        float max_val = fmaxf(fabsf(a->data[i]), fabsf(b->data[i]));
        float threshold = fmaxf(tolerance, tolerance * max_val);
        if (diff > threshold) {
            size_t row = i / a->cols;
            size_t col = i % a->cols;
            fprintf(stderr, "Mismatch at (%zu, %zu): %.8f vs %.8f "
                    "(diff = %.8e)\n",
                    row, col, a->data[i], b->data[i], diff);
            return false;
        }
    }

    return true;
}

/**
 * @brief Print a matrix to stdout (for small matrices only).
 *
 * If the matrix is larger than 16x16, only prints a summary.
 *
 * @param mat Pointer to the matrix to print.
 * @param name Optional name label for the matrix.
 */
static inline void matrix_print(const Matrix *mat, const char *name) {
    if (mat == NULL) {
        printf("%s: NULL\n", name ? name : "Matrix");
        return;
    }

    printf("%s (%zu x %zu):\n", name ? name : "Matrix", mat->rows, mat->cols);

    if (mat->rows > 16 || mat->cols > 16) {
        printf("  [matrix too large to print, showing corners]\n");
        size_t show = 4;
        for (size_t i = 0; i < show && i < mat->rows; i++) {
            printf("  ");
            for (size_t j = 0; j < show && j < mat->cols; j++) {
                printf("%10.4f ", mat->data[i * mat->cols + j]);
            }
            printf(" ... ");
            for (size_t j = (mat->cols > show ? mat->cols - show : 0);
                 j < mat->cols; j++) {
                printf("%10.4f ", mat->data[i * mat->cols + j]);
            }
            printf("\n");
        }
        printf("  ...\n");
        return;
    }

    for (size_t i = 0; i < mat->rows; i++) {
        printf("  ");
        for (size_t j = 0; j < mat->cols; j++) {
            printf("%10.4f ", mat->data[i * mat->cols + j]);
        }
        printf("\n");
    }
}

#endif // MATRIX_H
