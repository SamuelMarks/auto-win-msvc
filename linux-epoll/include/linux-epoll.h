#ifndef LINUX_EPOLL_H
#define LINUX_EPOLL_H

/**
 * @file linux-epoll.h
 * @brief Polyfill for Linux <sys/epoll.h> I/O event notification facility.
 */

/* clang-format off */
#include <stddef.h>
#if defined(_WIN32) && !defined(__CYGWIN__) && (!defined(_MSC_VER) || _MSC_VER >= 1600)
#include <stdint.h>
#include <wepoll.h>
#elif defined(__linux__)
#include <sys/epoll.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by linux-epoll functions.
 */
enum linux_epoll_error_code {
  /** @brief Successful operation. */
  LINUX_EPOLL_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  LINUX_EPOLL_ERROR_NULL_POINTER = 1,
  /** @brief Invalid argument or descriptor passed to function. */
  LINUX_EPOLL_ERROR_INVALID_ARGUMENT = 2,
  /** @brief Epoll functionality not supported on this platform. */
  LINUX_EPOLL_ERROR_NOT_SUPPORTED = 3
};

/**
 * @brief Initializes and validates the linux-epoll module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return LINUX_EPOLL_SUCCESS on success, or an error code on failure.
 */
enum linux_epoll_error_code linux_epoll_init(int *out_status);

#if defined(_WIN32) && !defined(__CYGWIN__) &&                                 \
    (!defined(_MSC_VER) || _MSC_VER >= 1600)

/**
 * @brief Creates an epoll instance.
 * @param size Size hint (ignored in modern implementations).
 * @return File descriptor for epoll instance, or -1 on error.
 */
int posix_epoll_create(int size);

/**
 * @brief Creates an epoll instance with flags.
 * @param flags Bitmask of flags (e.g. EPOLL_CLOEXEC).
 * @return File descriptor for epoll instance, or -1 on error.
 */
int posix_epoll_create1(int flags);

/**
 * @brief Control interface for an epoll descriptor.
 * @param epfd Epoll file descriptor.
 * @param op Operation (EPOLL_CTL_ADD, EPOLL_CTL_MOD, EPOLL_CTL_DEL).
 * @param fd Target file descriptor.
 * @param event Event structure pointer.
 * @return 0 on success, or -1 on error.
 */
int posix_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

/**
 * @brief Waits for events on an epoll instance.
 * @param epfd Epoll file descriptor.
 * @param events Buffer for returning ready events.
 * @param maxevents Maximum number of events to return.
 * @param timeout Timeout in milliseconds (-1 for infinite).
 * @return Number of ready events, or -1 on error.
 */
int posix_epoll_wait(int epfd, struct epoll_event *events, int maxevents,
                     int timeout);

/**
 * @brief Closes an epoll instance.
 * @param epfd Epoll file descriptor.
 * @return 0 on success, or -1 on error.
 */
int posix_epoll_close(int epfd);

#define epoll_create posix_epoll_create
#define epoll_create1 posix_epoll_create1
#define epoll_ctl posix_epoll_ctl
#define epoll_wait posix_epoll_wait
#define epoll_close posix_epoll_close
#elif defined(_WIN32) && !defined(__CYGWIN__)
/**
 * @brief Creates an epoll instance.
 * @param size Size hint.
 * @return File descriptor for epoll instance, or -1 on error.
 */
int posix_epoll_create(int size);

/**
 * @brief Creates an epoll instance with flags.
 * @param flags Bitmask of flags.
 * @return File descriptor for epoll instance, or -1 on error.
 */
int posix_epoll_create1(int flags);

/**
 * @brief Control interface for an epoll descriptor.
 * @param epfd Epoll file descriptor.
 * @param op Operation.
 * @param fd Target file descriptor.
 * @param event Event pointer.
 * @return 0 on success, or -1 on error.
 */
int posix_epoll_ctl(int epfd, int op, int fd, void *event);

/**
 * @brief Waits for events on an epoll instance.
 * @param epfd Epoll file descriptor.
 * @param events Buffer for events.
 * @param maxevents Maximum events.
 * @param timeout Timeout in milliseconds.
 * @return Number of events, or -1 on error.
 */
int posix_epoll_wait(int epfd, void *events, int maxevents, int timeout);

/**
 * @brief Closes an epoll instance.
 * @param epfd Epoll file descriptor.
 * @return 0 on success, or -1 on error.
 */
int posix_epoll_close(int epfd);

#define epoll_create posix_epoll_create
#define epoll_create1 posix_epoll_create1
#define epoll_ctl posix_epoll_ctl
#define epoll_wait posix_epoll_wait
#define epoll_close posix_epoll_close
#elif defined(__linux__)
/* Native Linux sys/epoll.h included */
#else
/**
 * @brief Creates an epoll instance stub.
 * @param size Size hint.
 * @return -1 with errno set to ENOSYS.
 */
int posix_epoll_create(int size);

/**
 * @brief Creates an epoll instance stub.
 * @param flags Bitmask of flags.
 * @return -1 with errno set to ENOSYS.
 */
int posix_epoll_create1(int flags);

/**
 * @brief Control interface stub for an epoll descriptor.
 * @param epfd Epoll file descriptor.
 * @param op Operation.
 * @param fd Target file descriptor.
 * @param event Event pointer.
 * @return -1 with errno set to ENOSYS.
 */
int posix_epoll_ctl(int epfd, int op, int fd, void *event);

/**
 * @brief Waits for events stub.
 * @param epfd Epoll file descriptor.
 * @param events Buffer for events.
 * @param maxevents Maximum events.
 * @param timeout Timeout in milliseconds.
 * @return -1 with errno set to ENOSYS.
 */
int posix_epoll_wait(int epfd, void *events, int maxevents, int timeout);

/**
 * @brief Closes an epoll instance stub.
 * @param epfd Epoll file descriptor.
 * @return -1 with errno set to ENOSYS.
 */
int posix_epoll_close(int epfd);

#define epoll_create posix_epoll_create
#define epoll_create1 posix_epoll_create1
#define epoll_ctl posix_epoll_ctl
#define epoll_wait posix_epoll_wait
#define epoll_close posix_epoll_close
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_EPOLL_H */
