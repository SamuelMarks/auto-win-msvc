#ifndef POSIX_POLL_H
#define POSIX_POLL_H

/**
 * @file posix-poll.h
 * @brief POSIX poll.h implementation for MSVC.
 */

/* clang-format off */
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#else
#include <poll.h>
#endif
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
#if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0600)
/**
 * @brief Poll file descriptor structure for Windows.
 */
struct pollfd {
  /** @brief File or socket descriptor. */
  SOCKET fd;
  /** @brief Requested event flags. */
  short events;
  /** @brief Returned event flags. */
  short revents;
};
#endif
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
 * @brief Error codes returned by posix-poll functions.
 */
enum posix_poll_error_code {
  /** @brief Operation completed successfully. */
  POSIX_POLL_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_POLL_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on posix-poll availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_POLL_SUCCESS on success, or POSIX_POLL_ERROR_NULL_POINTER on
 * NULL pointer.
 */
enum posix_poll_error_code posix_poll_get_info(int *out_available);

/**
 * @brief Poll a set of file descriptors for I/O readiness.
 * @return Number of ready descriptors, 0 on timeout, or -1 on error.
 */
int posix_poll(struct pollfd *fds, unsigned long nfds, int timeout);

#ifndef poll
#define poll posix_poll
#endif
#ifndef WSAPoll
#define WSAPoll posix_poll
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_POLL_H */
