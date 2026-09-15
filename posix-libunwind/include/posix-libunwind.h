#ifndef POSIX_LIBUNWIND_H
#define POSIX_LIBUNWIND_H

/**
 * @file posix-libunwind.h
 * @brief Polyfill and error handling interface for libunwind operations.
 */

/* clang-format off */
#include "libunwind.h"
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by posix-libunwind functions.
 */
enum posix_libunwind_error_code {
  /** @brief Operation completed successfully. */
  POSIX_LIBUNWIND_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_LIBUNWIND_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on posix-libunwind availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_LIBUNWIND_SUCCESS on success, or
 * POSIX_LIBUNWIND_ERROR_NULL_POINTER on NULL pointer.
 */
enum posix_libunwind_error_code posix_libunwind_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_LIBUNWIND_H */
