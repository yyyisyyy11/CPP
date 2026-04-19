#include "vabs.h"
#include <cmath>

bool vabs(int *p, size_t n)
{
    if (p == nullptr)
        return false;
    for (size_t i = 0; i < n; i++)
        p[i] = abs(p[i]);
    return true;
}

bool vabs(float *p, size_t n)
{
    if (p == nullptr)
        return false;
    for (size_t i = 0; i < n; i++)
        p[i] = fabsf(p[i]);
    return true;
}

bool vabs(double *p, size_t n)
{
    if (p == nullptr)
        return false;
    for (size_t i = 0; i < n; i++)
        p[i] = fabs(p[i]);
    return true;
}
