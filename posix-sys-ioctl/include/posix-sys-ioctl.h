/* posix-sys-ioctl.h - Strict C89 Header */
#ifndef POSIX_SYS_IOCTL_H
#define POSIX_SYS_IOCTL_H

/**
 * @file posix-sys-ioctl.h
 * @brief POSIX sys/ioctl.h implementation for MSVC
 *
 * This header provides the POSIX ioctl function
 * mapping to Winsock's ioctlsocket API.
 */

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Performs device-specific control functions on a socket.
 *
 * @param fd The file descriptor/socket on which to perform the operation.
 * @param request The operation code to perform.
 * @param ... Additional arguments.
 * @return 0 on success, -1 on error with errno set appropriately.
 */
int posix_ioctl(int fd, unsigned long request, ...);

#ifdef __cplusplus
}
#endif

#else /* Not MSVC/Windows */

#include <sys/ioctl.h>

#endif /* defined(_MSC_VER) || defined(_WIN32) */

#endif /* POSIX_SYS_IOCTL_H */
