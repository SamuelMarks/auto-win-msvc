#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/**
 * @file linux-epoll.c
 * @brief Implementation of Linux <sys/epoll.h> compatibility layer.
 */

/* clang-format off */
#include "sys/epoll.h"
#include "auto-win-msvc-error.h"
#include <errno.h>
#include <stddef.h>
#if defined(_WIN32)
#include <stdlib.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1400
#include <crtdbg.h>
#endif
#endif

#undef epoll_create
#undef epoll_create1
#undef epoll_ctl
#undef epoll_wait
#undef epoll_close

#if (defined(_MSC_VER) && _MSC_VER >= 1600) || defined(__MINGW32__) || defined(__MINGW64__)
#undef EPOLLIN
#undef EPOLLPRI
#undef EPOLLOUT
#undef EPOLLERR
#undef EPOLLHUP
#undef EPOLLRDNORM
#undef EPOLLRDBAND
#undef EPOLLWRNORM
#undef EPOLLWRBAND
#undef EPOLLMSG
#undef EPOLLRDHUP
#undef EPOLLEXCLUSIVE
#undef EPOLLWAKEUP
#undef EPOLLONESHOT
#undef EPOLLET
#undef EPOLL_CLOEXEC
#undef EPOLL_CTL_ADD
#undef EPOLL_CTL_DEL
#undef EPOLL_CTL_MOD

#undef epoll_data
#undef epoll_event
#undef epoll_data_t
#define epoll_data wepoll_epoll_data
#define epoll_event wepoll_epoll_event
#define epoll_data_t wepoll_epoll_data_t
#include <wepoll.h>
#undef epoll_data
#undef epoll_event
#undef epoll_data_t

#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(_MSC_VER) && _MSC_VER < 1600
#ifndef _WINSOCK2API_
#include <winsock2.h>
#endif
#endif
/* clang-format on */

#ifndef LINUX_EPOLL_MOCK_get_osfhandle
#define LINUX_EPOLL_MOCK_get_osfhandle _get_osfhandle
#endif
#ifndef LINUX_EPOLL_MOCK_wepoll_create
#define LINUX_EPOLL_MOCK_wepoll_create epoll_create
#endif
#ifndef LINUX_EPOLL_MOCK_wepoll_create1
#define LINUX_EPOLL_MOCK_wepoll_create1 epoll_create1
#endif
#ifndef LINUX_EPOLL_MOCK_wepoll_ctl
#define LINUX_EPOLL_MOCK_wepoll_ctl epoll_ctl
#endif
#ifndef LINUX_EPOLL_MOCK_wepoll_wait
#define LINUX_EPOLL_MOCK_wepoll_wait epoll_wait
#endif
#ifndef LINUX_EPOLL_MOCK_wepoll_close
#define LINUX_EPOLL_MOCK_wepoll_close epoll_close
#endif

#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1400
static void epoll_null_invalid_param_handler(const wchar_t *expression,
                                             const wchar_t *function,
                                             const wchar_t *file,
                                             unsigned int line,
                                             uintptr_t pReserved) {
  (void)expression;
  (void)function;
  (void)file;
  (void)line;
  (void)pReserved;
}
#endif
static ptrdiff_t safe_get_osfhandle(intptr_t fd) {
  ptrdiff_t h = -1;
  int saved_err = errno;
  if (fd >= 2048) {
    return -1;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  {
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(epoll_null_invalid_param_handler);
    AUTO_WIN_MSVC_SUPPRESS_CRT_REPORT();
    h = (ptrdiff_t)LINUX_EPOLL_MOCK_get_osfhandle((int)fd);
    _set_invalid_parameter_handler(old);
  }
#else
  h = (ptrdiff_t)LINUX_EPOLL_MOCK_get_osfhandle((int)fd);
#endif
  errno = saved_err;
  if (h == -2) {
    return -1;
  }
  return h;
}
#else
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)(fd))
#endif
#endif

#if (defined(_MSC_VER) && _MSC_VER >= 1600) || defined(__MINGW32__) ||         \
    defined(__MINGW64__)
static HANDLE epoll_handles[1024];
static int next_epoll_fd = 100;

/**
 * @brief Creates a new epoll instance backed by wepoll.
 * @param size Ignored size hint (must be positive in legacy API).
 * @return Non-negative epoll descriptor on success, or -1 on error.
 */
int posix_epoll_create(int size) {
  int fd;
  HANDLE h = LINUX_EPOLL_MOCK_wepoll_create(size);
  if (h == NULL) {
    return -1;
  }
  fd = next_epoll_fd++;
  epoll_handles[fd] = h;
  return fd;
}

/**
 * @brief Creates a new epoll instance with flags backed by wepoll.
 * @param flags Creation flags.
 * @return Non-negative epoll descriptor on success, or -1 on error.
 */
int posix_epoll_create1(int flags) {
  int fd;
  HANDLE h = LINUX_EPOLL_MOCK_wepoll_create1(flags);
  if (h == NULL) {
    return -1;
  }
  fd = next_epoll_fd++;
  epoll_handles[fd] = h;
  return fd;
}

/**
 * @brief Controls an epoll descriptor registration on Windows.
 * @param epfd Epoll descriptor.
 * @param op Operation code (ADD=1, DEL=2, MOD=3).
 * @param fd Target socket or file descriptor.
 * @param event Event configuration structure.
 * @return 0 on success, or -1 on error with errno set.
 */
int posix_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) {
  HANDLE hephnd;
  SOCKET s;
  ptrdiff_t h;
  int wop = op;
  int res;
  struct epoll_event local_ev;
  struct epoll_event *ev_ptr = event;
  if (epfd < 100 || epfd >= 1024 || epoll_handles[epfd] == NULL || fd < 0) {
    errno = 9;
    return -1;
  }
  hephnd = epoll_handles[epfd];
  if (op == 2) {
    wop = 3;
  } else if (op == 3) {
    wop = 2;
  }
  if (event != NULL) {
    local_ev = *event;
    local_ev.events &= ~(uint32_t)(1U << 31);
    if (event->events & (1U << 30)) {
      local_ev.events |= (1U << 31);
    }
    ev_ptr = &local_ev;
  }
  h = safe_get_osfhandle(fd);
  s = (h != -1) ? (SOCKET)h : (SOCKET)(intptr_t)fd;
  res = LINUX_EPOLL_MOCK_wepoll_ctl(hephnd, wop, s, (void *)ev_ptr);
  if (res == -1 && s != (SOCKET)(intptr_t)fd) {
    res = LINUX_EPOLL_MOCK_wepoll_ctl(hephnd, wop, (SOCKET)(intptr_t)fd,
                                      (void *)ev_ptr);
  }
  return res;
}

/**
 * @brief Waits for events on a Windows epoll descriptor.
 * @param epfd Epoll descriptor.
 * @param[out] events Output array of ready events.
 * @param maxevents Maximum number of events to return.
 * @param timeout Timeout in milliseconds.
 * @return Number of ready descriptors, or -1 on error.
 */
int posix_epoll_wait(int epfd, struct epoll_event *events, int maxevents,
                     int timeout) {
  HANDLE hephnd;
  if (epfd < 100 || epfd >= 1024) {
    errno = 9;
    return -1;
  }
  hephnd = epoll_handles[epfd];
  return LINUX_EPOLL_MOCK_wepoll_wait(hephnd, (void *)events, maxevents,
                                      timeout);
}

/**
 * @brief Closes a Windows epoll descriptor.
 * @param epfd Epoll descriptor.
 * @return 0 on success, or negative error code on invalid descriptor.
 */
int posix_epoll_close(int epfd) {
  HANDLE hephnd;
  int res;
  if (epfd < 100 || epfd >= 1024 || epoll_handles[epfd] == NULL) {
    return -2;
  }
  hephnd = epoll_handles[epfd];
  res = LINUX_EPOLL_MOCK_wepoll_close(hephnd);
  epoll_handles[epfd] = NULL;
  return res;
}
#endif

#if (defined(_MSC_VER) && _MSC_VER < 1600) ||                                  \
    (!defined(_WIN32) && !defined(__linux__)) ||                               \
    (defined(_WIN32) && !defined(_MSC_VER) && !defined(__MINGW32__) &&         \
     !defined(__MINGW64__))
/**
 * @brief Fallback epoll_create returning ENOSYS/EINVAL.
 * @param size Size hint.
 * @return Always -1.
 */
int posix_epoll_create(int size) {
  if (size <= 0) {
    errno = EINVAL;
    return -1;
  }
  errno = ENOSYS;
  return -1;
}

/**
 * @brief Fallback epoll_create1 returning ENOSYS/EINVAL.
 * @param flags Creation flags.
 * @return Always -1.
 */
int posix_epoll_create1(int flags) {
  if (flags < 0) {
    errno = EINVAL;
    return -1;
  }
  errno = ENOSYS;
  return -1;
}

/**
 * @brief Fallback epoll_ctl returning ENOSYS/EINVAL.
 * @param epfd Epoll descriptor.
 * @param op Control operation.
 * @param fd Target file descriptor.
 * @param event Event pointer.
 * @return Always -1.
 */
int posix_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) {
  if (epfd < 0 || fd < 0 || event == NULL) {
    errno = EINVAL;
    return -1;
  }
  if (op != 1 && op != 2 && op != 3) {
    errno = EINVAL;
    return -1;
  }
  errno = ENOSYS;
  return -1;
}

/**
 * @brief Fallback epoll_wait returning ENOSYS/EINVAL.
 * @param epfd Epoll descriptor.
 * @param[out] events Output events array.
 * @param maxevents Maximum events count.
 * @param timeout Timeout in milliseconds.
 * @return Always -1.
 */
int posix_epoll_wait(int epfd, struct epoll_event *events, int maxevents,
                     int timeout) {
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
 * @brief Fallback epoll_close returning ENOSYS/EBADF.
 * @param epfd Epoll descriptor.
 * @return Always -1.
 */
int posix_epoll_close(int epfd) {
  if (epfd < 0) {
    errno = EBADF;
    return -1;
  }
  errno = ENOSYS;
  return -1;
}
#endif

/**
 * @brief Initializes and validates the linux-epoll module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t linux_epoll_init(int *out_status) {
  if (out_status == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_status = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_epoll;

#endif
typedef int make_iso_compilers_happy_mega_batch2_15;
