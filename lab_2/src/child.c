#include <stdio.h>

int main() {
    while (1) {
        int a, b, c; 
        if (scanf("%d %d %d", &a, &b, &c) != 3) {
            break;
        }
        
        if (b == 0 || c == 0) {
            return 1;
        }
        
        int result = a / b / c;
        printf("%d\n", result);
    }
    return 0;
}