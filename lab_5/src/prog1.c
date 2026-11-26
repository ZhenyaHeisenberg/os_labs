#include <stdio.h>
#include "contracts.h"

int main(void) {
    char line[100];
    while (fgets(line, sizeof(line), stdin)) {
        int cmd;
        if (sscanf(line, "%d", &cmd) != 1) continue;

        if (cmd == 1) {
            float a, b, e;
            if (sscanf(line, "%d %f %f %f", &cmd, &a, &b, &e) == 4)
                printf("SinIntegral(%.4f, %.4f, %.4f) = %.6f\n",
                a, b, e, SinIntegral(a, b, e));
        } else if (cmd == 2) {
            int ia, ib;
            if (sscanf(line, "%d %d %d", &cmd, &ia, &ib) == 3)
                printf("PrimeCount(%d, %d) = %d\n",
                ia, ib, PrimeCount(ia, ib));
        }
    }
    return 0;
}
