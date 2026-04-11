#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * Dot product performance benchmark for multiple data types.
 * Single run per (type, n). Output: CSV lines "type,n,ms".
 *
 * Compile:
 *   gcc -o dotproduct dotproduct.c
 *   gcc -O3 -o dotproduct_O3 dotproduct.c
 */

/* --- Dot product functions for each data type --- */

float DotFloat(const float *a, const float *b, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

double DotDouble(const double *a, const double *b, int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

long long DotInt(const int *a, const int *b, int n) {
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        sum += (long long)a[i] * b[i];
    }
    return sum;
}

long long DotShort(const short *a, const short *b, int n) {
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        sum += (long long)a[i] * b[i];
    }
    return sum;
}

long long DotChar(const signed char *a, const signed char *b, int n) {
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        sum += (long long)a[i] * b[i];
    }
    return sum;
}

/* --- Helper: get current time in milliseconds --- */

double GetTimeMs(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}

/* --- Benchmark functions (single run) --- */

void BenchmarkFloat(int n) {
    float *a = (float *)malloc(n * sizeof(float));
    float *b = (float *)malloc(n * sizeof(float));
    if (a == NULL || b == NULL) {
        fprintf(stderr, "Memory allocation failed for n = %d\n", n);
        free(a); free(b); return;
    }

    for (int i = 0; i < n; i++) {
        a[i] = (float)rand() / RAND_MAX;
        b[i] = (float)rand() / RAND_MAX;
    }

    double start = GetTimeMs();
    volatile float result = DotFloat(a, b, n);
    double end = GetTimeMs();
    (void)result;
    printf("float,%d,%.6f\n", n, end - start);
    free(a); free(b);
}

void BenchmarkDouble(int n) {
    double *a = (double *)malloc(n * sizeof(double));
    double *b = (double *)malloc(n * sizeof(double));
    if (a == NULL || b == NULL) {
        fprintf(stderr, "Memory allocation failed for n = %d\n", n);
        free(a); free(b); return;
    }

    for (int i = 0; i < n; i++) {
        a[i] = (double)rand() / RAND_MAX;
        b[i] = (double)rand() / RAND_MAX;
    }

    double start = GetTimeMs();
    volatile double result = DotDouble(a, b, n);
    double end = GetTimeMs();
    (void)result;
    printf("double,%d,%.6f\n", n, end - start);
    free(a); free(b);
}

void BenchmarkInt(int n) {
    int *a = (int *)malloc(n * sizeof(int));
    int *b = (int *)malloc(n * sizeof(int));
    if (a == NULL || b == NULL) {
        fprintf(stderr, "Memory allocation failed for n = %d\n", n);
        free(a); free(b); return;
    }

    for (int i = 0; i < n; i++) {
        a[i] = rand() % 201 - 100;
        b[i] = rand() % 201 - 100;
    }

    double start = GetTimeMs();
    volatile long long result = DotInt(a, b, n);
    double end = GetTimeMs();
    (void)result;
    printf("int,%d,%.6f\n", n, end - start);
    free(a); free(b);
}

void BenchmarkShort(int n) {
    short *a = (short *)malloc(n * sizeof(short));
    short *b = (short *)malloc(n * sizeof(short));
    if (a == NULL || b == NULL) {
        fprintf(stderr, "Memory allocation failed for n = %d\n", n);
        free(a); free(b); return;
    }

    for (int i = 0; i < n; i++) {
        a[i] = (short)(rand() % 201 - 100);
        b[i] = (short)(rand() % 201 - 100);
    }

    double start = GetTimeMs();
    volatile long long result = DotShort(a, b, n);
    double end = GetTimeMs();
    (void)result;
    printf("short,%d,%.6f\n", n, end - start);
    free(a); free(b);
}

void BenchmarkChar(int n) {
    signed char *a = (signed char *)malloc(n * sizeof(signed char));
    signed char *b = (signed char *)malloc(n * sizeof(signed char));
    if (a == NULL || b == NULL) {
        fprintf(stderr, "Memory allocation failed for n = %d\n", n);
        free(a); free(b); return;
    }

    for (int i = 0; i < n; i++) {
        a[i] = (signed char)(rand() % 201 - 100);
        b[i] = (signed char)(rand() % 201 - 100);
    }

    double start = GetTimeMs();
    volatile long long result = DotChar(a, b, n);
    double end = GetTimeMs();
    (void)result;
    printf("char,%d,%.6f\n", n, end - start);
    free(a); free(b);
}

/* --- Main --- */

int main(void) {
    int sizes[] = {128, 1000, 10000, 100000, 1000000, 10000000, 100000000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    srand(42);  /* Fixed seed for reproducibility */

    /* CSV header */
    printf("type,n,ms\n");

    for (int i = 0; i < num_sizes; i++) BenchmarkFloat(sizes[i]);
    for (int i = 0; i < num_sizes; i++) BenchmarkDouble(sizes[i]);
    for (int i = 0; i < num_sizes; i++) BenchmarkInt(sizes[i]);
    for (int i = 0; i < num_sizes; i++) BenchmarkShort(sizes[i]);
    for (int i = 0; i < num_sizes; i++) BenchmarkChar(sizes[i]);

    return 0;
}
