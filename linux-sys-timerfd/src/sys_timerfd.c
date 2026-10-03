/* clang-format off */
#include "sys/timerfd.h"
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
#include <errno.h>

int timerfd_create(int clockid, int flags) {
  if (clockid < 0 || flags < 0) {
    errno = EINVAL;
    return -1;
  }
  errno = ENOSYS;
  return -1;
}

int timerfd_gettime(int fd, struct itimerspec *curr_value) {
  if (fd < 0 || curr_value == NULL) {
    errno = EINVAL;
    return -1;
  }
  errno = ENOSYS;
  return -1;
}

int timerfd_settime(int fd, int flags, const struct itimerspec *new_value,
                    struct itimerspec *old_value) {
  if (fd < 0 || flags < 0 || new_value == NULL) {
    errno = EINVAL;
    return -1;
  }
  if (old_value != NULL) {
    /* old_value provided */
  }
  errno = ENOSYS;
  return -1;
}

#endif /* _MSC_VER || _WIN32 */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_sys_timerfd_c;
