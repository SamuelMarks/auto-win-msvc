#ifndef LINUX_EPOLL_H
#define LINUX_EPOLL_H

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)

#if (defined(_MSC_VER) && _MSC_VER < 1600) || defined(__MINGW32__) || defined(__MINGW64__)

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
#define EPOLLEXCLUSIVE (1U << 28)
#define EPOLLWAKEUP (1U << 29)
#define EPOLLONESHOT (1U << 30)
#define EPOLLET (1U << 31)

#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3

typedef union epoll_data {
    void *ptr;
    int fd;
    unsigned int u32;
#if defined(_MSC_VER)
    unsigned __int64 u64;
#else
    __extension__ unsigned long long u64;
#endif
} epoll_data_t;

struct epoll_event {
    unsigned int events;
    epoll_data_t data;
};

int epoll_create(int size);
int epoll_create1(int flags);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);

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