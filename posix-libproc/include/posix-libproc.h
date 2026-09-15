#ifndef POSIX_LIBPROC_H
#define POSIX_LIBPROC_H

/**
 * @file posix-libproc.h
 * @brief Polyfill and process introspection functions for libproc.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by posix-libproc functions.
 */
enum posix_libproc_error_code {
  /** @brief Operation completed successfully. */
  POSIX_LIBPROC_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_LIBPROC_ERROR_NULL_POINTER = 1,
  /** @brief Provided buffer is insufficient to store path. */
  POSIX_LIBPROC_ERROR_BUFFER_TOO_SMALL = 2
};

/**
 * @brief Retrieves information on posix-libproc availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_LIBPROC_SUCCESS on success, or
 * POSIX_LIBPROC_ERROR_NULL_POINTER on NULL pointer.
 */
enum posix_libproc_error_code posix_libproc_get_info(int *out_available);

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
