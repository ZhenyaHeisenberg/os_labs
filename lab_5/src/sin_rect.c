#include <math.h>
#include "contracts.h"

float SinIntegral(float A, float B, float e) {
    if (e <= 0.0f || A >= B) return 0.0f;
    float sum = 0.0f;
    for (float x = A; x < B; x += e)
        sum += sinf(x + e / 2.0f);
    return sum * e;
}