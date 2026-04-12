#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * Dot product performance benchmark for multiple data types.
 * Uses adaptive iteration count: for small n, repeats the dot product
 * many times and reports the average, ensuring measurable times even
 * when a single call takes only nanoseconds.
 *
 * Timing: clock_gettime(CLOCK_MONOTONIC) for nanosecond-resolution
 * monotonic clock (immune to wall-clock adjustments).
 *
 * Output: CSV lines "type,n,ms"
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

/* --- Helper: get current time in nanoseconds (monotonic clock) --- */

static double GetTimeNs(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}

/*
 * Calculate number of iterations so that total runtime is roughly
 * TARGET_MS milliseconds, ensuring even tiny n values give accurate
 * measurements.  For large n a single iteration already takes long
 * enough, so iters == 1.
 */
#define TARGET_MS 50.0

static int CalcIterations(int n) {
    /*
     * Heuristic: assume ~1 ns per element (a rough upper bound on
     * modern hardware with -O3). We want total_ns >= TARGET_MS * 1e6.
     * iters = ceil(TARGET_MS * 1e6 / n).  Clamp to [1, 10000000].
     */
    double est_ns_per_call = (double)n;          /* ~1 ns/element */
    double target_ns = TARGET_MS * 1e6;
    int iters = (int)(target_ns / est_ns_per_call);
    if (iters < 1)       iters = 1;
    if (iters > 10000000) iters = 10000000;
    return iters;
}

/* --- Benchmark functions (adaptive iterations) --- */

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

    int iters = CalcIterations(n);
    volatile float result;

    double start = GetTimeNs();
    for (int it = 0; it < iters; it++) {
        result = DotFloat(a, b, n);
    }
    double end = GetTimeNs();
    (void)result;

    double avg_ms = (end - start) / iters / 1e6;
    printf("float,%d,%.8f\n", n, avg_ms);
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

    int iters = CalcIterations(n);
    volatile double result;

    double start = GetTimeNs();
    for (int it = 0; it < iters; it++) {
        result = DotDouble(a, b, n);
    }
    double end = GetTimeNs();
    (void)result;

    double avg_ms = (end - start) / iters / 1e6;
    printf("double,%d,%.8f\n", n, avg_ms);
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

    int iters = CalcIterations(n);
    volatile long long result;

    double start = GetTimeNs();
    for (int it = 0; it < iters; it++) {
        result = DotInt(a, b, n);
    }
    double end = GetTimeNs();
    (void)result;

    double avg_ms = (end - start) / iters / 1e6;
    printf("int,%d,%.8f\n", n, avg_ms);
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

    int iters = CalcIterations(n);
    volatile long long result;

    double start = GetTimeNs();
    for (int it = 0; it < iters; it++) {
        result = DotShort(a, b, n);
    }
    double end = GetTimeNs();
    (void)result;

    double avg_ms = (end - start) / iters / 1e6;
    printf("short,%d,%.8f\n", n, avg_ms);
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

    int iters = CalcIterations(n);
    volatile long long result;

    double start = GetTimeNs();
    for (int it = 0; it < iters; it++) {
        result = DotChar(a, b, n);
    }
    double end = GetTimeNs();
    (void)result;

    double avg_ms = (end - start) / iters / 1e6;
    printf("char,%d,%.8f\n", n, avg_ms);
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
