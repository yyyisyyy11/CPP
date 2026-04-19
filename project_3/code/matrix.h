#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>
#include <stdbool.h>

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
Matrix *matrix_create(size_t rows, size_t cols);

/**
 * @brief Free the memory associated with a matrix.
 *
 * Safely handles NULL pointers.
 *
 * @param mat Pointer to the matrix to free.
 */
void matrix_free(Matrix *mat);

/**
 * @brief Fill a matrix with random float values in [0, 1).
 *
 * @param mat Pointer to the matrix to fill.
 */
void matrix_random(Matrix *mat);

/**
 * @brief Fill a matrix with zeros.
 *
 * @param mat Pointer to the matrix to fill.
 */
void matrix_zero(Matrix *mat);

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
bool matrix_equal(const Matrix *a, const Matrix *b, float tolerance);

/**
 * @brief Print a matrix to stdout (for small matrices only).
 *
 * If the matrix is larger than 16x16, only prints a summary.
 *
 * @param mat Pointer to the matrix to print.
 * @param name Optional name label for the matrix.
 */
void matrix_print(const Matrix *mat, const char *name);

/**
 * @brief Plain matrix multiplication: C = A * B.
 *
 * Uses a straightforward triple-nested loop (i, j, k order).
 * This serves as the correctness benchmark.
 *
 * @param a Input matrix A (rows_a x cols_a).
 * @param b Input matrix B (rows_b x cols_b). Requires cols_a == rows_b.
 * @param c Output matrix C (rows_a x cols_b). Must be pre-allocated.
 * @return true on success, false on invalid parameters.
 */
bool matmul_plain(const Matrix *a, const Matrix *b, Matrix *c);

/**
 * @brief Improved matrix multiplication: C = A * B.
 *
 * Uses loop reordering (i, k, j) and cache tiling for better
 * cache locality. Future versions will add SIMD and OpenMP.
 *
 * @param a Input matrix A (rows_a x cols_a).
 * @param b Input matrix B (rows_b x cols_b). Requires cols_a == rows_b.
 * @param c Output matrix C (rows_a x cols_b). Must be pre-allocated.
 * @return true on success, false on invalid parameters.
 */
bool matmul_improved(const Matrix *a, const Matrix *b, Matrix *c);

#endif // MATRIX_H
