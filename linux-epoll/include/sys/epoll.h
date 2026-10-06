/* clang-format off */
#if defined(__linux__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif
#include_next <sys/epoll.h>
#else
#include <stdint.h>
#endif
/* clang-format on */

#if !defined(__linux__)
#ifndef AUTO_WIN_MSVC_SYS_EPOLL_H
#define AUTO_WIN_MSVC_SYS_EPOLL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if !defined(_MSC_VER) || _MSC_VER < 1600 ||                                   \
    (!defined(__MINGW32__) && !defined(__MINGW64__))
#ifndef EPOLL_CTL_ADD
#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3
#endif
#endif

#ifndef EPOLL_EVENTS_DEFINED
#define EPOLL_EVENTS_DEFINED
#if (!defined(_MSC_VER) || _MSC_VER < 1600) &&                                 \
    (!defined(__MINGW32__) && !defined(__MINGW64__))
enum EPOLL_EVENTS {
  EPOLLIN = 0x001,
  EPOLLPRI = 0x002,
  EPOLLOUT = 0x004,
  EPOLLERR = 0x008,
  EPOLLHUP = 0x010,
  EPOLLRDNORM = 0x040,
  EPOLLRDBAND = 0x080,
  EPOLLWRNORM = 0x100,
  EPOLLWRBAND = 0x200,
  EPOLLMSG = 0x400,
  EPOLLRDHUP = 0x2000,
  EPOLLONESHOT = (1u << 30)
};
#endif
#endif

typedef union epoll_data {
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;

struct epoll_event {
  uint32_t events;
  epoll_data_t data;
};

#define epoll_create posix_epoll_create
#define epoll_create1 posix_epoll_create1
#define epoll_ctl posix_epoll_ctl
#define epoll_wait posix_epoll_wait
#define epoll_close posix_epoll_close
#define epoll_pwait posix_epoll_pwait
#define epoll_pwait2 posix_epoll_pwait2

int posix_epoll_create(int size);
int posix_epoll_create1(int flags);
int posix_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int posix_epoll_wait(int epfd, struct epoll_event *events, int maxevents,
                     int timeout);
int posix_epoll_close(int epfd);
int posix_epoll_pwait(int epfd, struct epoll_event *events, int maxevents,
                      int timeout, const void *sigmask);
int posix_epoll_pwait2(int epfd, struct epoll_event *events, int maxevents,
                       const void *timeout, const void *sigmask);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AUTO_WIN_MSVC_SYS_EPOLL_H */
#endif
/* API Contract symbols: __dependencies__ */
