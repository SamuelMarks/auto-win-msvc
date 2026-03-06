#include <stdio.h>

int wait(void) { return 42; }

int main(void) {
    printf("%d\n", wait());
    return 0;
}
