#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
int main() {
  fchmodat(AT_FDCWD, ".", 0, 0);
  return 0;
}
