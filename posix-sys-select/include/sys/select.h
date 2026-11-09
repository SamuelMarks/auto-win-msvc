#ifndef POSIX_SYS_SELECT_SYS_SELECT_H
#if defined(__GNUC__)
#endif
#define POSIX_SYS_SELECT_SYS_SELECT_H

/**
 * @file select.h
 * @brief POSIX sys/select.h header and MSVC fd_set helpers.
 */

/* clang-format off */
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#elif defined(__MSDOS__) || defined(__WATCOMC__) || defined(__DOS__)
#include <stddef.h>
#else
#if !defined(__APPLE__)
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/select.h>
#else
#include <sys/select.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__MSDOS__) || defined(__WATCOMC__) || defined(__DOS__)
#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED
#define _STRUCT_TIMEVAL
/** @brief Time value structure for select timeouts on DOS. */
struct timeval {
  long tv_sec;  /**< Seconds. */
  long tv_usec; /**< Microseconds. */
};
#endif

#ifndef FD_SETSIZE
/** @brief Maximum number of descriptors in an fd_set on DOS. */
#define FD_SETSIZE 64
#endif

#ifndef _FD_SET_DEFINED
#define _FD_SET_DEFINED
/** @brief File descriptor set structure for select on DOS. */
typedef struct fd_set {
  unsigned int fd_count;    /**< Number of descriptors in the set. */
  int fd_array[FD_SETSIZE]; /**< Array of file descriptors. */
} fd_set;
#endif

#ifndef FD_ZERO
/** @brief Clear all descriptors from an fd_set. */
#define FD_ZERO(set) (((fd_set *)(set))->fd_count = 0)
#endif
#ifndef FD_SET
/** @brief Add a descriptor to an fd_set. */
#define FD_SET(fd, set)                                                        \
  do {                                                                         \
    if (((fd_set *)(set))->fd_count < FD_SETSIZE) {                            \
      ((fd_set *)(set))->fd_array[((fd_set *)(set))->fd_count++] = (int)(fd);  \
    }                                                                          \
  } while (0)
#endif
#ifndef FD_CLR
/** @brief Remove a descriptor from an fd_set. */
#define FD_CLR(fd, set) ((void)(fd), (void)(set))
#endif
#ifndef FD_ISSET
/** @brief Test if a descriptor is part of an fd_set. */
#define FD_ISSET(fd, set) 0
#endif

/** @brief Synchronous I/O multiplexing stub for DOS. */
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
           struct timeval *timeout);
#endif

#if defined(_WIN32)

#if defined(_MSC_VER)
#undef FD_ZERO
#define FD_ZERO(set) (((fd_set *)(set))->fd_count = 0)
#undef FD_SET
#define FD_SET(fd, set) auto_win_msvc_fd_set((SOCKET)(fd), (fd_set *)(set))
#undef FD_CLR
#define FD_CLR(fd, set) auto_win_msvc_fd_clr((SOCKET)(fd), (fd_set *)(set))
#endif

/**
 * @brief Adds a socket descriptor to an fd_set without exceeding FD_SETSIZE.
 * @param fd Socket descriptor to add.
 * @param set Pointer to fd_set.
 */
void auto_win_msvc_fd_set(SOCKET fd, fd_set *set);

/**
 * @brief Removes a socket descriptor from an fd_set.
 * @param fd Socket descriptor to remove.
 * @param set Pointer to fd_set.
 */
void auto_win_msvc_fd_clr(SOCKET fd, fd_set *set);

#endif /* defined(_WIN32) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_SELECT_SYS_SELECT_H */
