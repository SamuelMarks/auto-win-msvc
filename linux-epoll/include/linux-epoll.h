#ifndef LINUX_EPOLL_H
#define LINUX_EPOLL_H

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)

#include <stdint.h>
#include <wepoll.h>

#ifdef __cplusplus
extern "C" {
#endif

int posix_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
#define epoll_ctl posix_epoll_ctl

#ifdef __cplusplus
}
#endif

#else
#include <sys/epoll.h>
#endif /* _MSC_VER */

#endif /* LINUX_EPOLL_H */
