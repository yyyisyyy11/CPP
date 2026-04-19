#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

Matrix *matrix_create(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0) {
        fprintf(stderr, "Error: matrix dimensions must be non-zero.\n");
        return NULL;
    }

    Matrix *mat = (Matrix *)malloc(sizeof(Matrix));
    if (mat == NULL) {
        fprintf(stderr, "Error: failed to allocate Matrix struct.\n");
        return NULL;
    }

    mat->rows = rows;
    mat->cols = cols;

    /* Use calloc to zero-initialize the data */
    mat->data = (float *)calloc(rows * cols, sizeof(float));
    if (mat->data == NULL) {
        fprintf(stderr, "Error: failed to allocate matrix data (%zu x %zu).\n",
                rows, cols);
        free(mat);
        return NULL;
    }

    return mat;
}

void matrix_free(Matrix *mat) {
    if (mat != NULL) {
        free(mat->data);
        mat->data = NULL;
        mat->rows = 0;
        mat->cols = 0;
        free(mat);
    }
}

void matrix_random(Matrix *mat) {
    if (mat == NULL || mat->data == NULL) {
        fprintf(stderr, "Error: cannot fill NULL matrix with random values.\n");
        return;
    }

    size_t total = mat->rows * mat->cols;
    for (size_t i = 0; i < total; i++) {
        mat->data[i] = (float)rand() / (float)RAND_MAX;
    }
}

void matrix_zero(Matrix *mat) {
    if (mat == NULL || mat->data == NULL) {
        fprintf(stderr, "Error: cannot zero NULL matrix.\n");
        return;
    }

    memset(mat->data, 0, mat->rows * mat->cols * sizeof(float));
}

bool matrix_equal(const Matrix *a, const Matrix *b, float tolerance) {
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

void matrix_print(const Matrix *mat, const char *name) {
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
