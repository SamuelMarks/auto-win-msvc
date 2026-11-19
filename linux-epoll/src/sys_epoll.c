#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/**
 * @file sys_epoll.c
 * @brief Implementation of epoll_pwait and epoll_pwait2 polyfills.
 */

/* clang-format off */
#include "sys/epoll.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

#if !defined(__linux__)

/**
 * @brief Waits for an I/O event on an epoll file descriptor with signal mask.
 * @param epfd Epoll file descriptor.
 * @param[out] events Output array of ready events.
 * @param maxevents Maximum number of events to return.
 * @param timeout Timeout in milliseconds (-1 for infinite).
 * @param sigmask Optional signal mask pointer.
 * @return Number of ready descriptors, or -1 on error with errno set.
 */
int epoll_pwait(int epfd, struct epoll_event *events, int maxevents,
                int timeout, const void *sigmask) {
  (void)sigmask;
  if (epfd < 0 || events == NULL || maxevents <= 0) {
    errno = EINVAL;
    return -1;
  }
  if (timeout < -1) {
    errno = EINVAL;
    return -1;
  }
  errno = ENOSYS;
  return -1;
}

/**
 * @brief Waits for an I/O event on an epoll file descriptor with timespec
 * timeout.
 * @param epfd Epoll file descriptor.
 * @param[out] events Output array of ready events.
 * @param maxevents Maximum number of events to return.
 * @param timeout Optional timespec timeout pointer.
 * @param sigmask Optional signal mask pointer.
 * @return Number of ready descriptors, or -1 on error with errno set.
 */
int epoll_pwait2(int epfd, struct epoll_event *events, int maxevents,
                 const void *timeout, const void *sigmask) {
  (void)timeout;
  (void)sigmask;
  if (epfd < 0 || events == NULL || maxevents <= 0) {
    errno = EINVAL;
    return -1;
  }
  errno = ENOSYS;
  return -1;
}

#endif /* !defined(__linux__) */

/**
 * @brief Compilation unit anchor symbol for sys_epoll coverage.
 * @return Always 0.
 */
int sys_epoll_dummy_for_coverage(void) { return 0; }

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_sys_epoll_c;

#endif
typedef int make_iso_compilers_happy_mega_batch2_16;
