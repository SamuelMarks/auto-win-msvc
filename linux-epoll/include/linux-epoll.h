/* linux-epoll.h - Strict C89 Header */
#ifndef AUTO_WIN_MSVC_LINUX_EPOLL_H
#define AUTO_WIN_MSVC_LINUX_EPOLL_H

/**
 * @file linux-epoll.h
 * @brief Scalable I/O event notification polyfill (<sys/epoll.h>) for MSVC and
 * non-Linux platforms.
 */

/* clang-format off */
#if defined(__linux__)
#include <sys/epoll.h>
#include <unistd.h>
#else
#if defined(_MSC_VER) && _MSC_VER < 1600
#ifndef _UINT32_T_DEFINED
#define _UINT32_T_DEFINED
typedef unsigned int uint32_t;
#endif
#ifndef _UINT64_T_DEFINED
#define _UINT64_T_DEFINED
typedef unsigned __int64 uint64_t;
#endif
#else
#include <stdint.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if !defined(__linux__)

#ifndef EPOLL_CLOEXEC
/** @brief Set close-on-exec flag on the new epoll file descriptor. */
#define EPOLL_CLOEXEC 02000000
#endif

#ifndef EPOLL_CTL_ADD
/** @brief Add a file descriptor to the epoll interest list. */
#define EPOLL_CTL_ADD 1
/** @brief Remove a file descriptor from the epoll interest list. */
#define EPOLL_CTL_DEL 2
/** @brief Change the settings associated with a file descriptor in the interest
 * list. */
#define EPOLL_CTL_MOD 3
#endif

#ifndef EPOLLIN
/** @brief The associated file is available for read(2) operations. */
#define EPOLLIN 0x001
/** @brief There is an exceptional condition on the file descriptor. */
#define EPOLLPRI 0x002
/** @brief The associated file is available for write(2) operations. */
#define EPOLLOUT 0x004
/** @brief Error condition happened on the associated file descriptor. */
#define EPOLLERR 0x008
/** @brief Hang up happened on the associated file descriptor. */
#define EPOLLHUP 0x010
/** @brief Normal data may be read. */
#define EPOLLRDNORM 0x040
/** @brief Priority band data may be read. */
#define EPOLLRDBAND 0x080
/** @brief Writing now will not block. */
#define EPOLLWRNORM 0x100
/** @brief Priority data may be written. */
#define EPOLLWRBAND 0x200
/** @brief Message available. */
#define EPOLLMSG 0x400
/** @brief Stream socket peer closed connection, or shut down writing half of
 * connection. */
#define EPOLLRDHUP 0x2000
/** @brief Sets an exclusive wakeup mode for the epoll file descriptor. */
#define EPOLLEXCLUSIVE (1U << 28)
/** @brief Ensures system does not enter suspend while event is pending. */
#define EPOLLWAKEUP (1U << 29)
/** @brief Sets the one-shot behavior for the associated file descriptor. */
#define EPOLLONESHOT (1U << 30)
/** @brief Sets the Edge Triggered behavior for the associated file descriptor.
 */
#define EPOLLET (1U << 31)
#endif

#ifndef AUTO_WIN_MSVC_EPOLL_TYPES_DEFINED
#define AUTO_WIN_MSVC_EPOLL_TYPES_DEFINED
/**
 * @brief User data variable associated with an epoll event.
 */
typedef union epoll_data {
  void *ptr;    /**< Pointer to user-defined data. */
  int fd;       /**< File descriptor. */
  uint32_t u32; /**< 32-bit unsigned integer. */
  uint64_t u64; /**< 64-bit unsigned integer. */
} epoll_data_t;

/**
 * @brief Structure describing an epoll event.
 */
struct epoll_event {
  uint32_t events;   /**< Epoll events bitmask. */
  epoll_data_t data; /**< User data variable. */
};
#endif /* AUTO_WIN_MSVC_EPOLL_TYPES_DEFINED */

#ifndef epoll_create
/** @brief Maps epoll_create to posix_epoll_create. */
#define epoll_create posix_epoll_create
/** @brief Maps epoll_create1 to posix_epoll_create1. */
#define epoll_create1 posix_epoll_create1
/** @brief Maps epoll_ctl to posix_epoll_ctl. */
#define epoll_ctl posix_epoll_ctl
/** @brief Maps epoll_wait to posix_epoll_wait. */
#define epoll_wait posix_epoll_wait
/** @brief Maps epoll_close to posix_epoll_close. */
#define epoll_close posix_epoll_close
/** @brief Maps epoll_pwait to posix_epoll_pwait. */
#define epoll_pwait posix_epoll_pwait
/** @brief Maps epoll_pwait2 to posix_epoll_pwait2. */
#define epoll_pwait2 posix_epoll_pwait2
#endif

/**
 * @brief Creates a new epoll instance.
 * @param[in] size Ignored, but must be greater than zero.
 * @return File descriptor referring to the new epoll instance, or -1 on error.
 */
int posix_epoll_create(int size);

/**
 * @brief Creates a new epoll instance with flags.
 * @param[in] flags Bitmask of flags (e.g. EPOLL_CLOEXEC).
 * @return File descriptor referring to the new epoll instance, or -1 on error.
 */
int posix_epoll_create1(int flags);

/**
 * @brief Control interface for an epoll file descriptor.
 * @param[in] epfd Epoll file descriptor.
 * @param[in] op Operation to perform (EPOLL_CTL_ADD, EPOLL_CTL_MOD,
 * EPOLL_CTL_DEL).
 * @param[in] fd Target file descriptor.
 * @param[in] event Event configuration associated with fd.
 * @return 0 on success, or -1 on error.
 */
int posix_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

/**
 * @brief Waits for an I/O event on an epoll file descriptor.
 * @param[in] epfd Epoll file descriptor.
 * @param[out] events Buffer receiving ready events.
 * @param[in] maxevents Maximum number of events to return.
 * @param[in] timeout Timeout in milliseconds (-1 to block indefinitely).
 * @return Number of ready file descriptors, 0 on timeout, or -1 on error.
 */
int posix_epoll_wait(int epfd, struct epoll_event *events, int maxevents,
                     int timeout);

/**
 * @brief Closes an epoll file descriptor.
 * @param[in] epfd Epoll file descriptor.
 * @return 0 on success, or -1 on error.
 */
int posix_epoll_close(int epfd);

/**
 * @brief Waits for an I/O event on an epoll file descriptor with signal mask.
 * @param[in] epfd Epoll file descriptor.
 * @param[out] events Buffer receiving ready events.
 * @param[in] maxevents Maximum number of events to return.
 * @param[in] timeout Timeout in milliseconds.
 * @param[in] sigmask Signal mask to atomically apply during wait.
 * @return Number of ready file descriptors, 0 on timeout, or -1 on error.
 */
int posix_epoll_pwait(int epfd, struct epoll_event *events, int maxevents,
                      int timeout, const void *sigmask);

/**
 * @brief Waits for an I/O event on an epoll file descriptor with timespec
 * timeout.
 * @param[in] epfd Epoll file descriptor.
 * @param[out] events Buffer receiving ready events.
 * @param[in] maxevents Maximum number of events to return.
 * @param[in] timeout Pointer to struct timespec specifying timeout.
 * @param[in] sigmask Signal mask to atomically apply during wait.
 * @return Number of ready file descriptors, 0 on timeout, or -1 on error.
 */
int posix_epoll_pwait2(int epfd, struct epoll_event *events, int maxevents,
                       const void *timeout, const void *sigmask);

#endif /* !defined(__linux__) */

#if defined(__linux__)
#ifndef epoll_close
/** @brief Maps epoll_close to standard close(2) on Linux. */
#define epoll_close close
#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AUTO_WIN_MSVC_LINUX_EPOLL_H */
