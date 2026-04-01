/* clang-format off */
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
/* clang-format on */
int main() {
  fchmodat(AT_FDCWD, ".", 0, 0);
  return 0;
}
