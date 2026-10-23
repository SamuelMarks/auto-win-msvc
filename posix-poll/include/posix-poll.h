#ifndef POSIX_POLL_H
#define POSIX_POLL_H

/**
 * @file posix-poll.h
 * @brief POSIX poll.h implementation for MSVC.
 */

#if !defined(_WIN32) && !defined(_MSC_VER) && !defined(__APPLE__)
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif
#endif

/* clang-format off */
#include "auto-win-msvc-error.h"
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#else
#include <poll.h>
#include <signal.h>
#endif
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
#define pollfd posix_pollfd
/**
 * @brief Poll file descriptor structure for Windows.
 */
struct posix_pollfd {
  /** @brief File or socket descriptor. */
  int fd;
  /** @brief Requested event flags. */
  short events;
  /** @brief Returned event flags. */
  short revents;
};
#endif

#if defined(_WIN32)
#ifndef _NFDS_T_DEFINED
#define _NFDS_T_DEFINED
typedef unsigned long nfds_t;
#endif

#ifndef _SIGSET_T_DEFINED
#define _SIGSET_T_DEFINED
typedef unsigned long sigset_t;
#endif
#endif

#if defined(_WIN32)
#undef POLLIN
#undef POLLPRI
#undef POLLOUT
#undef POLLERR
#undef POLLHUP
#undef POLLNVAL
#endif

#ifndef POLLIN
/** @brief There is data to read. */
#define POLLIN 0x01
/** @brief There is urgent data to read. */
#define POLLPRI 0x02
/** @brief Writing now will not block. */
#define POLLOUT 0x04
/** @brief Error condition. */
#define POLLERR 0x08
/** @brief Hung up. */
#define POLLHUP 0x10
/** @brief Invalid request: fd not open. */
#define POLLNVAL 0x20
#endif

/**
 * @brief Retrieves information on posix-poll availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_poll_get_info(int *out_available);

/**
 * @brief Poll a set of file descriptors for I/O readiness.
 * @return Number of ready descriptors, 0 on timeout, or -1 on error.
 */
int posix_poll(struct pollfd *fds, unsigned long nfds, int timeout);

struct timespec;

/**
 * @brief Poll a set of file descriptors for I/O readiness with timespec timeout
 * and signal mask.
 * @return Number of ready descriptors, 0 on timeout, or -1 on error.
 */
int posix_ppoll(struct pollfd *fds, unsigned long nfds,
                const struct timespec *tmo_p, const sigset_t *sigmask);

#ifndef poll
#define poll posix_poll
#endif
#ifndef WSAPoll
#define WSAPoll posix_poll
#endif
#ifndef ppoll
#define ppoll posix_ppoll
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_POLL_H */
