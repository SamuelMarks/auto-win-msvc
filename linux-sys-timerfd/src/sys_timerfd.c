/* clang-format off */
#include "sys/timerfd.h"
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
#include <errno.h>

int timerfd_create(int clockid, int flags) {
  (void)clockid;
  (void)flags;
  errno = ENOSYS;
  return -1;
}

int timerfd_gettime(int fd, struct itimerspec *curr_value) {
  (void)fd;
  (void)curr_value;
  errno = ENOSYS;
  return -1;
}

int timerfd_settime(int fd, int flags, const struct itimerspec *new_value,
                    struct itimerspec *old_value) {
  (void)fd;
  (void)flags;
  (void)new_value;
  (void)old_value;
  errno = ENOSYS;
  return -1;
}

#endif /* _MSC_VER || _WIN32 */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_sys_timerfd_c;
