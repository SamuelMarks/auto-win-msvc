/* clang-format off */
#include "linux-epoll.h"

#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
#include <stddef.h>
#if defined(_WIN32)
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)_get_osfhandle(fd))
#else
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)(fd))
#endif
#endif
#undef epoll_create
#undef epoll_create1
#undef epoll_ctl
#undef epoll_wait
#undef epoll_close

#if (defined(_MSC_VER) && _MSC_VER >= 1600) || defined(__MINGW32__) ||         \
    defined(__MINGW64__)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <wepoll.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#endif

#if (defined(_MSC_VER) && _MSC_VER < 1600) ||                                  \
    (!defined(_WIN32) && !defined(__linux__)) ||                               \
    (defined(_WIN32) && !defined(_MSC_VER) && !defined(__MINGW32__) &&         \
     !defined(__MINGW64__))
#include <errno.h>
#endif
/* clang-format on */

#if (defined(_MSC_VER) && _MSC_VER >= 1600) || defined(__MINGW32__) ||         \
    defined(__MINGW64__)
static HANDLE epoll_handles[1024];
static int next_epoll_fd = 100;

int posix_epoll_create(int size) {
  int fd;
  HANDLE h = epoll_create(size);
  if (h == NULL)
    return -1;
  fd = next_epoll_fd++;
  epoll_handles[fd] = h;
  return fd;
}

int posix_epoll_create1(int flags) {
  int fd;
  HANDLE h = epoll_create1(flags);
  if (h == NULL)
    return -1;
  fd = next_epoll_fd++;
  epoll_handles[fd] = h;
  return fd;
}

int posix_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) {
  HANDLE hephnd = epoll_handles[epfd];
  SOCKET s;
  if (fd < 0) {
    errno = 9;
    return -1;
  }
  s = (SOCKET)safe_get_osfhandle(fd);
  return epoll_ctl(hephnd, op, s, event);
}

int posix_epoll_wait(int epfd, struct epoll_event *events, int maxevents,
                     int timeout) {
  HANDLE hephnd = epoll_handles[epfd];
  return epoll_wait(hephnd, events, maxevents, timeout);
}

int posix_epoll_close(int epfd) {
  HANDLE hephnd = epoll_handles[epfd];
  int res = epoll_close(hephnd);
  epoll_handles[epfd] = NULL;
  return res;
}
#endif

#if (defined(_MSC_VER) && _MSC_VER < 1600) ||                                  \
    (!defined(_WIN32) && !defined(__linux__)) ||                               \
    (defined(_WIN32) && !defined(_MSC_VER) && !defined(__MINGW32__) &&         \
     !defined(__MINGW64__))
int posix_epoll_create(int size) {
  (void)size;
  errno = ENOSYS;
  return -1;
}
int posix_epoll_create1(int flags) {
  (void)flags;
  errno = ENOSYS;
  return -1;
}
int posix_epoll_ctl(int epfd, int op, int fd, void *event) {
  (void)epfd;
  (void)op;
  (void)fd;
  (void)event;
  errno = ENOSYS;
  return -1;
}
int posix_epoll_wait(int epfd, void *events, int maxevents, int timeout) {
  (void)epfd;
  (void)events;
  (void)maxevents;
  (void)timeout;
  errno = ENOSYS;
  return -1;
}
int posix_epoll_close(int epfd) {
  (void)epfd;
  errno = ENOSYS;
  return -1;
}
#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;
