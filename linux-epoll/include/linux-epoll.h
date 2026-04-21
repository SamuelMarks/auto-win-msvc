#ifndef LINUX_EPOLL_H
#define LINUX_EPOLL_H

#if defined(_WIN32) && !defined(__CYGWIN__) &&                                 \
    (!defined(_MSC_VER) || _MSC_VER >= 1600)
#include <stdint.h>
#include <wepoll.h>
#ifdef __cplusplus
extern "C" {
#endif
/** \brief posix_epoll_create function. */
int posix_epoll_create(int size);
/** \brief posix_epoll_create1 function. */
int posix_epoll_create1(int flags);
/** \brief posix_epoll_ctl function. */
int posix_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
/** \brief posix_epoll_wait function. */
int posix_epoll_wait(int epfd, struct epoll_event *events, int maxevents,
                     int timeout);
/** \brief posix_epoll_close function. */
int posix_epoll_close(int epfd);
#define epoll_create posix_epoll_create
#define epoll_create1 posix_epoll_create1
#define epoll_ctl posix_epoll_ctl
#define epoll_wait posix_epoll_wait
#ifdef __cplusplus
}
#endif
#elif defined(_WIN32) && !defined(__CYGWIN__)
#ifdef __cplusplus
extern "C" {
#endif
/** \brief posix_epoll_create function. */
int posix_epoll_create(int size);
/** \brief posix_epoll_create1 function. */
int posix_epoll_create1(int flags);
/** \brief posix_epoll_ctl function. */
int posix_epoll_ctl(int epfd, int op, int fd, void *event);
/** \brief posix_epoll_wait function. */
int posix_epoll_wait(int epfd, void *events, int maxevents, int timeout);
/** \brief posix_epoll_close function. */
int posix_epoll_close(int epfd);
#define epoll_create posix_epoll_create
#define epoll_create1 posix_epoll_create1
#define epoll_ctl posix_epoll_ctl
#define epoll_wait posix_epoll_wait
#ifdef __cplusplus
}
#endif
#elif defined(__linux__) || (defined(__CYGWIN__) && defined(__x86_64__))
#include <sys/epoll.h>
#else
#ifdef __cplusplus
extern "C" {
#endif
/** \brief posix_epoll_create function. */
int posix_epoll_create(int size);
/** \brief posix_epoll_create1 function. */
int posix_epoll_create1(int flags);
/** \brief posix_epoll_ctl function. */
int posix_epoll_ctl(int epfd, int op, int fd, void *event);
/** \brief posix_epoll_wait function. */
int posix_epoll_wait(int epfd, void *events, int maxevents, int timeout);
/** \brief posix_epoll_close function. */
int posix_epoll_close(int epfd);
#define epoll_create posix_epoll_create
#define epoll_create1 posix_epoll_create1
#define epoll_ctl posix_epoll_ctl
#define epoll_wait posix_epoll_wait
#ifdef __cplusplus
}
#endif
#endif

#endif /* LINUX_EPOLL_H */
