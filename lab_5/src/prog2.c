#include <stdio.h>
#include <windows.h>
#include "contracts.h"

typedef float (*sin_fn)(float, float, float);
typedef int   (*prime_fn)(int, int);

static const char *SIN_DLLS[]   = { "sin_rect.dll",    "sin_trap.dll"    };
static const char *PRIME_DLLS[] = { "prime_naive.dll", "prime_eratosthenes.dll" };

typedef struct {
    HMODULE  h_sin;
    HMODULE  h_prime;
    sin_fn   sin_ptr;
    prime_fn prime_ptr;
} LibSet;

static int load_libs(LibSet *ls, int idx) {
    ls->h_sin = LoadLibraryA(SIN_DLLS[idx]);
    if (!ls->h_sin) { fprintf(stderr, "LoadLibrary sin failed: %lu\n", GetLastError()); return -1; }

    ls->h_prime = LoadLibraryA(PRIME_DLLS[idx]);
    if (!ls->h_prime) {
        fprintf(stderr, "LoadLibrary prime failed: %lu\n", GetLastError());
        FreeLibrary(ls->h_sin); ls->h_sin = NULL; return -1;
    }

    ls->sin_ptr   = (sin_fn)   GetProcAddress(ls->h_sin,   "SinIntegral");
    ls->prime_ptr = (prime_fn) GetProcAddress(ls->h_prime, "PrimeCount");

    if (!ls->sin_ptr || !ls->prime_ptr) {
        fprintf(stderr, "GetProcAddress failed\n");
        FreeLibrary(ls->h_sin); FreeLibrary(ls->h_prime);
        ls->h_sin = ls->h_prime = NULL; return -1;
    }
    return 0;
}

static void unload_libs(LibSet *ls) {
    if (ls->h_sin)   FreeLibrary(ls->h_sin);
    if (ls->h_prime) FreeLibrary(ls->h_prime);
    ls->h_sin = ls->h_prime = NULL;
    ls->sin_ptr = NULL; ls->prime_ptr = NULL;
}

int main(void) {
    LibSet ls = {0};
    int idx = 0;
    if (load_libs(&ls, idx) != 0) return 1;
    printf("Loaded implementation #%d\n", idx + 1);

    char line[100];
    while (fgets(line, sizeof(line), stdin)) {
        int cmd;
        if (sscanf(line, "%d", &cmd) != 1) continue;

        if (cmd == 0) {
            unload_libs(&ls);
            idx = (idx + 1) % 2;
            if (load_libs(&ls, idx) != 0) return 1;
            printf("Switched to implementation #%d\n", idx + 1);
        } else if (cmd == 1) {
            float a, b, e;
            if (sscanf(line, "%d %f %f %f", &cmd, &a, &b, &e) == 4)
                printf("SinIntegral(%.4f, %.4f, %.4f) = %.6f\n",
                a, b, e, ls.sin_ptr(a, b, e));
        } else if (cmd == 2) {
            int ia, ib;
            if (sscanf(line, "%d %d %d", &cmd, &ia, &ib) == 3)
                printf("PrimeCount(%d, %d) = %d\n",
                ia, ib, ls.prime_ptr(ia, ib));
        }
    }
    unload_libs(&ls);
    return 0;
}
