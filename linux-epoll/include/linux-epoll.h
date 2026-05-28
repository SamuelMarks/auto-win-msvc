#ifndef LINUX_EPOLL_H
#define LINUX_EPOLL_H
#include "auto_win_msvc_export.h"

#if defined(_WIN32) && !defined(__CYGWIN__) &&                                 \
    (!defined(_MSC_VER) || _MSC_VER >= 1600)
/* clang-format off */
#include <stdint.h>
#include <wepoll.h>
#elif defined(__linux__)
#include <sys/epoll.h>
/* clang-format on */
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) && !defined(__CYGWIN__) &&                                 \
    (!defined(_MSC_VER) || _MSC_VER >= 1600)
/** \brief posix_epoll_create function. */
AUTO_WIN_MSVC_EXPORT int posix_epoll_create(int size);
/** \brief posix_epoll_create1 function. */
AUTO_WIN_MSVC_EXPORT int posix_epoll_create1(int flags);
/** \brief posix_epoll_ctl function. */
AUTO_WIN_MSVC_EXPORT int posix_epoll_ctl(int epfd, int op, int fd,
                                         struct epoll_event *event);
/** \brief posix_epoll_wait function. */
AUTO_WIN_MSVC_EXPORT int posix_epoll_wait(int epfd, struct epoll_event *events,
                                          int maxevents, int timeout);
/** \brief posix_epoll_close function. */
AUTO_WIN_MSVC_EXPORT int posix_epoll_close(int epfd);
#define epoll_create posix_epoll_create
#define epoll_create1 posix_epoll_create1
#define epoll_ctl posix_epoll_ctl
#define epoll_wait posix_epoll_wait
#elif defined(_WIN32) && !defined(__CYGWIN__)
/** \brief posix_epoll_create function. */
AUTO_WIN_MSVC_EXPORT int posix_epoll_create(int size);
/** \brief posix_epoll_create1 function. */
AUTO_WIN_MSVC_EXPORT int posix_epoll_create1(int flags);
/** \brief posix_epoll_ctl function. */
AUTO_WIN_MSVC_EXPORT int posix_epoll_ctl(int epfd, int op, int fd, void *event);
/** \brief posix_epoll_wait function. */
AUTO_WIN_MSVC_EXPORT int posix_epoll_wait(int epfd, void *events, int maxevents,
                                          int timeout);
/** \brief posix_epoll_close function. */
AUTO_WIN_MSVC_EXPORT int posix_epoll_close(int epfd);
#define epoll_create posix_epoll_create
#define epoll_create1 posix_epoll_create1
#define epoll_ctl posix_epoll_ctl
#define epoll_wait posix_epoll_wait
#elif defined(__linux__)
/* Includes moved to the top */
#else
/** \brief posix_epoll_create function. */
AUTO_WIN_MSVC_EXPORT int posix_epoll_create(int size);
/** \brief posix_epoll_create1 function. */
AUTO_WIN_MSVC_EXPORT int posix_epoll_create1(int flags);
/** \brief posix_epoll_ctl function. */
AUTO_WIN_MSVC_EXPORT int posix_epoll_ctl(int epfd, int op, int fd, void *event);
/** \brief posix_epoll_wait function. */
AUTO_WIN_MSVC_EXPORT int posix_epoll_wait(int epfd, void *events, int maxevents,
                                          int timeout);
/** \brief posix_epoll_close function. */
AUTO_WIN_MSVC_EXPORT int posix_epoll_close(int epfd);
#define epoll_create posix_epoll_create
#define epoll_create1 posix_epoll_create1
#define epoll_ctl posix_epoll_ctl
#define epoll_wait posix_epoll_wait
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_EPOLL_H */
