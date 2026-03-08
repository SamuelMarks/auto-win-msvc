/* posix-sys-uio.h - Strict C89 Header */
#ifndef POSIX_SYS_UIO_H
#define POSIX_SYS_UIO_H

/**
 * @file posix-sys-uio.h
 * @brief POSIX sys/uio.h implementation for MSVC
 *
 * This header provides the POSIX readv and writev functions
 * mapped to WSASend and _write system calls.
 */

#if defined(_MSC_VER) || defined(_WIN32)

#include <stddef.h> /* size_t */

/**
 * @brief Vector structure for scatter/gather I/O operations.
 */
struct iovec {
    void *iov_base; /* Base address */
    size_t iov_len; /* Length */
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reads data into multiple buffers.
 *
 * @param fd The file descriptor/socket to read from.
 * @param iov A pointer to an array of iovec structures.
 * @param iovcnt The number of elements in the iov array.
 * @return On success, the total bytes read. On error, -1 with errno set appropriately.
 */
long posix_readv(int fd, const struct iovec *iov, int iovcnt);

/**
 * @brief Writes data from multiple buffers.
 *
 * @param fd The file descriptor/socket to write to.
 * @param iov A pointer to an array of iovec structures.
 * @param iovcnt The number of elements in the iov array.
 * @return On success, the total bytes written. On error, -1 with errno set appropriately.
 */
long posix_writev(int fd, const struct iovec *iov, int iovcnt);

#ifdef __cplusplus
}
#endif

#else /* Not MSVC/Windows */

#include <sys/uio.h>

#endif /* defined(_MSC_VER) || defined(_WIN32) */

#endif /* POSIX_SYS_UIO_H */
