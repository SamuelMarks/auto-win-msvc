#ifndef LINUX_EPOLL_H
#define LINUX_EPOLL_H

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__) ||       \
    defined(__CYGWIN__) || defined(__WATCOMC__)

#if (defined(_MSC_VER) && _MSC_VER < 1600) || defined(__MINGW32__) ||          \
    defined(__MINGW64__) || defined(__CYGWIN__) || defined(__WATCOMC__)

#ifdef __cplusplus
extern "C" {
#endif

#define EPOLLIN 0x001
#define EPOLLPRI 0x002
#define EPOLLOUT 0x004
#define EPOLLERR 0x008
#define EPOLLHUP 0x010
#define EPOLLRDNORM 0x040
#define EPOLLRDBAND 0x080
#define EPOLLWRNORM 0x100
#define EPOLLWRBAND 0x200
#define EPOLLMSG 0x400
#define EPOLLRDHUP 0x2000
/** \brief EPOLLEXCLUSIVE macro. */
#define EPOLLEXCLUSIVE (1U << 28)
/** \brief EPOLLWAKEUP macro. */
#define EPOLLWAKEUP (1U << 29)
/** \brief EPOLLONESHOT macro. */
#define EPOLLONESHOT (1U << 30)
/** \brief EPOLLET macro. */
#define EPOLLET (1U << 31)

#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3

typedef union epoll_data {
  void *ptr;
  int fd;
  unsigned int u32;
#if defined(_MSC_VER) || defined(__WATCOMC__)
  unsigned __int64 u64;
#else
  __extension__ unsigned long long u64;
#endif
} epoll_data_t;

struct epoll_event {
  unsigned int events;
  epoll_data_t data;
};

/** \brief epoll_create function. */
int epoll_create(int size);
/** \brief epoll_create1 function. */
int epoll_create1(int flags);
/** \brief epoll_ctl function. */
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
/** \brief epoll_wait function. */
int epoll_wait(int epfd, struct epoll_event *events, int maxevents,
               int timeout);

#ifdef __cplusplus
}
#endif

#else
#include <wepoll.h>
#endif /* _MSC_VER < 1600 */

#else
#include <sys/epoll.h>
#endif /* _MSC_VER */

#endif /* LINUX_EPOLL_H */