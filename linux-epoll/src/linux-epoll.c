#include <linux-epoll.h>

#if (defined(_MSC_VER) && _MSC_VER < 1600) || defined(__MINGW32__) || defined(__MINGW64__)
#include <errno.h>

/** \brief epoll_create function. */
int epoll_create(int size) {
    (void)size;
    errno = ENOSYS;
    return -1;
}

/** \brief epoll_create1 function. */
int epoll_create1(int flags) {
    (void)flags;
    errno = ENOSYS;
    return -1;
}

/** \brief epoll_ctl function. */
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) {
    (void)epfd;
    (void)op;
    (void)fd;
    (void)event;
    errno = ENOSYS;
    return -1;
}

/** \brief epoll_wait function. */
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout) {
    (void)epfd;
    (void)events;
    (void)maxevents;
    (void)timeout;
    errno = ENOSYS;
    return -1;
}

#endif /* _MSC_VER < 1600 */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;
typedef int make_iso_compilers_happy_tu_linux_epoll;

