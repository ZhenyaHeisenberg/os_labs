#include "contracts.h"

static int isPrime(int n) {
    if (n < 2) return 0;
    for (int d = 2; (long long)d * d <= n; d++)
        if (n % d == 0) return 0;
    return 1;
}

int PrimeCount(int A, int B) {
    if (A > B) return 0;
    int cnt = 0;
    for (int n = A; n <= B; n++) cnt += isPrime(n);
    return cnt;
}