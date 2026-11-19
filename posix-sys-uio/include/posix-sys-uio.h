/* posix-sys-uio.h - Strict C89 Header */
#ifndef POSIX_SYS_UIO_H
#define POSIX_SYS_UIO_H

/**
 * @file posix-sys-uio.h
 * @brief POSIX sys/uio.h implementation for MSVC and Windows.
 *
 * Provides scatter/gather I/O functions (readv, writev, preadv, pwritev)
 * mapped to WSARecv/WSASend and CRT file I/O on Windows, and native sys/uio.h
 * on POSIX systems.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#if defined(_MSC_VER) || defined(_WIN32)
#include <stddef.h>
#elif defined(__MSDOS__) || defined(__WATCOMC__)
#include <stddef.h>
#else
#include <sys/types.h>
#include <sys/uio.h>
#endif

#if !defined(_MSC_VER) && (defined(_WIN32) || defined(__MSDOS__) || defined(__WATCOMC__))
#include <sys/types.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Retrieves information on posix-sys-uio module availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_sys_uio_get_info(int *out_available);

#if defined(_MSC_VER)
/** @brief 64-bit file offset type for preadv/pwritev on MSVC. */
typedef __int64 posix_uio_off_t;
#else
/** @brief Platform file offset type for preadv/pwritev. */
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
  void *iov_base; /**< Base address of buffer */
  size_t iov_len; /**< Length of buffer in bytes */
};
#endif
#endif

#ifndef IOV_MAX
/**
 * @brief Maximum number of iovec structures in a single operation.
 */
#define IOV_MAX 1024
#endif

#ifndef readv
/** @brief Alias readv to posix_readv on Windows/DOS. */
#define readv posix_readv
#endif
#ifndef writev
/** @brief Alias writev to posix_writev on Windows/DOS. */
#define writev posix_writev
#endif
#ifndef preadv
/** @brief Alias preadv to posix_preadv on Windows/DOS. */
#define preadv posix_preadv
#endif
#ifndef pwritev
/** @brief Alias pwritev to posix_pwritev on Windows/DOS. */
#define pwritev posix_pwritev
#endif

#endif /* defined(_MSC_VER) || defined(_WIN32) */

struct iovec;

/**
 * @brief Reads data into multiple buffers.
 * @param[in] fd File descriptor or socket handle to read from.
 * @param[in] iov Pointer to an array of iovec structures.
 * @param[in] iovcnt Number of elements in the iov array.
 * @return Total bytes read on success, or -1 on error with errno set.
 */
long posix_readv(int fd, const struct iovec *iov, int iovcnt);

/**
 * @brief Writes data from multiple buffers.
 * @param[in] fd File descriptor or socket handle to write to.
 * @param[in] iov Pointer to an array of iovec structures.
 * @param[in] iovcnt Number of elements in the iov array.
 * @return Total bytes written on success, or -1 on error with errno set.
 */
long posix_writev(int fd, const struct iovec *iov, int iovcnt);

/**
 * @brief Reads data into multiple buffers at a specified file offset.
 * @param[in] fd File descriptor to read from.
 * @param[in] iov Pointer to an array of iovec structures.
 * @param[in] iovcnt Number of elements in the iov array.
 * @param[in] offset File offset at which to read.
 * @return Total bytes read on success, or -1 on error with errno set.
 */
long posix_preadv(int fd, const struct iovec *iov, int iovcnt,
                  posix_uio_off_t offset);

/**
 * @brief Writes data from multiple buffers at a specified file offset.
 * @param[in] fd File descriptor to write to.
 * @param[in] iov Pointer to an array of iovec structures.
 * @param[in] iovcnt Number of elements in the iov array.
 * @param[in] offset File offset at which to write.
 * @return Total bytes written on success, or -1 on error with errno set.
 */
long posix_pwritev(int fd, const struct iovec *iov, int iovcnt,
                   posix_uio_off_t offset);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_UIO_H */
