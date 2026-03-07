#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
int main() {
    fchmodat(AT_FDCWD, ".", 0, 0);
    return 0;
}
