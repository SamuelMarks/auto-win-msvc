/* clang-format off */
#include "sys/epoll.h"
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
#include <errno.h>
#include <stddef.h>

int epoll_pwait(int epfd, struct epoll_event *events, int maxevents,
                int timeout, const void *sigmask) {
  if (epfd < 0 || events == NULL || maxevents <= 0) {
    errno = EINVAL;
    return -1;
  }
  if (timeout < -1) {
    errno = EINVAL;
    return -1;
  }
  if (sigmask != NULL) {
    /* sigmask provided */
  }
  errno = ENOSYS;
  return -1;
}

int epoll_pwait2(int epfd, struct epoll_event *events, int maxevents,
                 const void *timeout, const void *sigmask) {
  if (epfd < 0 || events == NULL || maxevents <= 0) {
    errno = EINVAL;
    return -1;
  }
  if (timeout != NULL || sigmask != NULL) {
    /* valid */
  }
  errno = ENOSYS;
  return -1;
}

#endif /* _MSC_VER || _WIN32 */

int sys_epoll_dummy_for_coverage(void) { return 0; }

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_sys_epoll_c;
