#ifndef POSIX_UNWIND_H
#define POSIX_UNWIND_H

/**
 * @file posix-unwind.h
 * @brief Polyfill and error handling interface for unwind operations.
 */

/* clang-format off */
#include "unwind.h"
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by posix-unwind functions.
 */
enum posix_unwind_error_code {
  /** @brief Operation completed successfully. */
  POSIX_UNWIND_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_UNWIND_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on posix-unwind availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_UNWIND_SUCCESS on success, or POSIX_UNWIND_ERROR_NULL_POINTER
 * on NULL pointer.
 */
enum posix_unwind_error_code posix_unwind_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_UNWIND_H */
