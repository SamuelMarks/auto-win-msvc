#ifndef LINUX_EPOLL_H
#define LINUX_EPOLL_H
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
#include <stdint.h>
#include <wepoll.h>
#ifdef __cplusplus
extern "C" {
#endif
int posix_epoll_create(int size);
int posix_epoll_create1(int flags);
int posix_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int posix_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
int posix_epoll_close(int epfd);
#define epoll_create posix_epoll_create
#define epoll_create1 posix_epoll_create1
#define epoll_ctl posix_epoll_ctl
#define epoll_wait posix_epoll_wait
#ifdef __cplusplus
}
#endif
#else
#include <sys/epoll.h>
#endif
#endif
