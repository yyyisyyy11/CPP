#pragma once
#include <cstddef>

// Overloaded vabs functions for int, float, double arrays
// Returns false if pointer is null, true otherwise
// size_t is better than int: it's unsigned, matches array indexing semantics,
// and avoids negative size values.

bool vabs(int *p, size_t n);
bool vabs(float *p, size_t n);
bool vabs(double *p, size_t n);
