#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/* clang-format off */
#include "sys/epoll.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

#if !defined(__linux__)

/**
 * @brief Waits for an I/O event on an epoll file descriptor with signal mask.
 */
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

/**
 * @brief Waits for an I/O event on an epoll file descriptor with timespec
 * timeout.
 */
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

#endif /* !defined(__linux__) */

int sys_epoll_dummy_for_coverage(void) { return 0; }

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_sys_epoll_c;

#endif
typedef int make_iso_compilers_happy_mega_batch2_16;
