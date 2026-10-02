/* clang-format off */
#include "sys/eventfd.h"
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
#include <errno.h>

int eventfd(unsigned int initval, int flags) {
  (void)initval;
  (void)flags;
  errno = ENOSYS;
  return -1;
}

#endif /* _MSC_VER || _WIN32 */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_sys_eventfd_c;
