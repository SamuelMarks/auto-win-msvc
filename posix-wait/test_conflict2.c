#include <stdio.h>
#include <sys/wait.h>

pid_t wait(int *stat_loc) { 
    (void)stat_loc;
    return 42; 
}

int main(void) {
    printf("%d\n", wait(NULL));
    return 0;
}
