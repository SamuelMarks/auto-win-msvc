#ifndef POSIX_SYS_IOCTL_H
#define POSIX_SYS_IOCTL_H

/**
 * @file posix-sys-ioctl.h
 * @brief POSIX sys/ioctl.h implementation for MSVC.
 */

/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#elif defined(__MSDOS__) || defined(__WATCOMC__)
#include <stddef.h>
#else
#include <sys/ioctl.h>
#endif
#include <stddef.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TIOCGWINSZ
/** @brief Get window size. */
#define TIOCGWINSZ 0x5413
#endif

#ifndef TIOCSWINSZ
/** @brief Set window size. */
#define TIOCSWINSZ 0x5414
#endif

#ifndef FIONREAD
/** @brief Get number of bytes available to read. */
#define FIONREAD 0x4004667f
#endif

#ifndef FIONBIO
/** @brief Set/clear non-blocking I/O. */
#define FIONBIO 0x8004667e
#endif

#if defined(_MSC_VER) || defined(_WIN32) || defined(__MSDOS__) ||              \
    defined(__WATCOMC__)
/**
 * @brief Window size structure for terminal/console ioctl operations.
 */
struct winsize {
  /** @brief Rows, in characters. */
  unsigned short ws_row;
  /** @brief Columns, in characters. */
  unsigned short ws_col;
  /** @brief Horizontal size, in pixels. */
  unsigned short ws_xpixel;
  /** @brief Vertical size, in pixels. */
  unsigned short ws_ypixel;
};
#endif

/**
 * @brief Error codes returned by posix-sys-ioctl functions.
 */
enum posix_sys_ioctl_error_code {
  /** @brief Operation completed successfully. */
  POSIX_SYS_IOCTL_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_SYS_IOCTL_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on posix-sys-ioctl availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_SYS_IOCTL_SUCCESS on success, or
 * POSIX_SYS_IOCTL_ERROR_NULL_POINTER on NULL pointer.
 */
enum posix_sys_ioctl_error_code posix_sys_ioctl_get_info(int *out_available);

/**
 * @brief Performs device-specific control functions on a socket or file.
 * @param fd The file descriptor or socket.
 * @param request The operation code to perform.
 * @param ... Additional arguments.
 * @return 0 on success, or -1 on error.
 */
int posix_ioctl(intptr_t fd, unsigned long request, ...);

#ifndef ioctl
#define ioctl posix_ioctl
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_IOCTL_H */
