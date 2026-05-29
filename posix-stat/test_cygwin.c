/* clang-format off */
#include <fcntl.h>
#include <sys/stat.h>
#if !defined(_MSC_VER)
#include <unistd.h>
#endif
/* clang-format on */
int main() {
  fchmodat(AT_FDCWD, ".", 0, 0);
  return 0;
}
