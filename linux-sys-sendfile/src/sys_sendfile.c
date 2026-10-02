/* clang-format off */
#include "sys/sendfile.h"
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
#include <errno.h>

ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count) {
  (void)out_fd;
  (void)in_fd;
  (void)offset;
  (void)count;
  errno = ENOSYS;
  return -1;
}

#endif /* _MSC_VER || _WIN32 */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_sys_sendfile_c;
