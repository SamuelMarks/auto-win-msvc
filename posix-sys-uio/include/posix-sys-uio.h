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
/* clang-format off */
#include <stddef.h> /* size_t */
#elif defined(__MSDOS__) || defined(__WATCOMC__)
#include <stddef.h> /* size_t */
#else
#include <sys/uio.h>
/* clang-format on */
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
typedef __int64 posix_uio_off_t;
#else
/* clang-format off */
#include <sys/types.h>
/* clang-format on */
typedef off_t posix_uio_off_t;
#endif

#if defined(_MSC_VER) || defined(_WIN32) || defined(__MSDOS__) ||              \
    defined(__WATCOMC__)

#ifndef _STRUCT_IOVEC_DEFINED
#ifndef AUTO_WIN_MSVC_SKIP_IOVEC
#define _STRUCT_IOVEC_DEFINED
/**
 * @struct iovec
 * @brief Vector structure for scatter/gather I/O operations.
 */
struct iovec {
  void *iov_base; /**< Base address */
  size_t iov_len; /**< Length in bytes */
};
#endif
#endif

#ifndef IOV_MAX
/**
 * @brief Maximum number of iovec structures in a single operation.
 */
#define IOV_MAX 1024
#endif

/**
 * @brief Reads data into multiple buffers.
 *
 * @param fd The file descriptor/socket to read from.
 * @param iov A pointer to an array of iovec structures.
 * @param iovcnt The number of elements in the iov array.
 * @return On success, the total bytes read. On error, -1 with errno set
 * appropriately.
 */
long posix_readv(int fd, const struct iovec *iov, int iovcnt);

/**
 * @brief Writes data from multiple buffers.
 *
 * @param fd The file descriptor/socket to write to.
 * @param iov A pointer to an array of iovec structures.
 * @param iovcnt The number of elements in the iov array.
 * @return On success, the total bytes written. On error, -1 with errno set
 * appropriately.
 */
long posix_writev(int fd, const struct iovec *iov, int iovcnt);

/**
 * @brief Reads data into multiple buffers at a specified file offset.
 *
 * @param fd The file descriptor to read from.
 * @param iov A pointer to an array of iovec structures.
 * @param iovcnt The number of elements in the iov array.
 * @param offset The 64-bit file offset to read from.
 * @return On success, the total bytes read. On error, -1 with errno set
 * appropriately.
 */
long posix_preadv(int fd, const struct iovec *iov, int iovcnt,
                  posix_uio_off_t offset);

/**
 * @brief Writes data from multiple buffers at a specified file offset.
 *
 * @param fd The file descriptor to write to.
 * @param iov A pointer to an array of iovec structures.
 * @param iovcnt The number of elements in the iov array.
 * @param offset The 64-bit file offset to write to.
 * @return On success, the total bytes written. On error, -1 with errno set
 * appropriately.
 */
long posix_pwritev(int fd, const struct iovec *iov, int iovcnt,
                   posix_uio_off_t offset);

#ifndef readv
#define readv posix_readv
#endif
#ifndef writev
#define writev posix_writev
#endif
#ifndef preadv
#define preadv posix_preadv
#endif
#ifndef pwritev
#define pwritev posix_pwritev
#endif

#else

long posix_readv(int fd, const struct iovec *iov, int iovcnt);
long posix_writev(int fd, const struct iovec *iov, int iovcnt);
long posix_preadv(int fd, const struct iovec *iov, int iovcnt,
                  posix_uio_off_t offset);
long posix_pwritev(int fd, const struct iovec *iov, int iovcnt,
                   posix_uio_off_t offset);

#endif /* defined(_MSC_VER) || defined(_WIN32) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_UIO_H */
