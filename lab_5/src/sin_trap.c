#include <math.h>
#include "contracts.h"

float SinIntegral(float A, float B, float e) {
    if (e <= 0.0f || A >= B) return 0.0f;
    int n = (int)((B - A) / e);
    if (n <= 0) return 0.0f;
    float sum = (sinf(A) + sinf(B)) / 2.0f;
    for (int i = 1; i < n; i++)
        sum += sinf(A + i * e);
    return sum * e;
}   