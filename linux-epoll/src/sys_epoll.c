/* clang-format off */
#include "sys/epoll.h"
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
#include <errno.h>

int epoll_pwait(int epfd, struct epoll_event *events, int maxevents,
                int timeout, const void *sigmask) {
  (void)epfd;
  (void)events;
  (void)maxevents;
  (void)timeout;
  (void)sigmask;
  errno = ENOSYS;
  return -1;
}

int epoll_pwait2(int epfd, struct epoll_event *events, int maxevents,
                 const void *timeout, const void *sigmask) {
  (void)epfd;
  (void)events;
  (void)maxevents;
  (void)timeout;
  (void)sigmask;
  errno = ENOSYS;
  return -1;
}

#endif /* _MSC_VER || _WIN32 */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_sys_epoll_c;
