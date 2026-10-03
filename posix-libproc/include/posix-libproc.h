#ifndef POSIX_LIBPROC_H
#define POSIX_LIBPROC_H

/**
 * @file posix-libproc.h
 * @brief Polyfill and process introspection functions for libproc.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Retrieves information on posix-libproc availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_libproc_get_info(int *out_available);

/**
 * @brief Retrieves the filesystem path for the given process ID.
 * @param pid Process ID (or 0 for current process on Windows).
 * @param buffer Output buffer to receive the path.
 * @param buffersize Size of output buffer in bytes.
 * @return Number of bytes written on success, or 0 on error.
 */
int proc_pidpath(int pid, void *buffer, unsigned int buffersize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_LIBPROC_H */
