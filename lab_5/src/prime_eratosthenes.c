#include <stdlib.h>
#include <string.h>
#include "contracts.h"

int PrimeCount(int A, int B) {
    if (A > B || B < 2) return 0;
    if (A < 2) A = 2;

    char *sieve = malloc((size_t)B + 1);
    if (!sieve) return -1;
    memset(sieve, 1, (size_t)B + 1);
    sieve[0] = sieve[1] = 0;

    for (int p = 2; (long long)p * p <= B; p++)
        if (sieve[p])
            for (long long m = (long long)p * p; m <= B; m += p)
                sieve[m] = 0;

    int cnt = 0;
    for (int n = A; n <= B; n++) cnt += sieve[n];
    free(sieve);
    return cnt;
}