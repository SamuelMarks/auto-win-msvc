
#ifndef POSIX_TIMES_H
#define POSIX_TIMES_H

/**
 * @file posix-times.h
 * @brief POSIX times and execution timing module.
 */

/* clang-format off */
#include "sys/times.h"
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by posix-times functions.
 */
enum posix_times_error_code {
  /** @brief Operation completed successfully. */
  POSIX_TIMES_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_TIMES_ERROR_NULL_POINTER = 1,
  /** @brief Failed to retrieve process times from underlying OS. */
  POSIX_TIMES_ERROR_SYSTEM = 2
};

/**
 * @brief Retrieves information on posix-times availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_TIMES_SUCCESS on success, or POSIX_TIMES_ERROR_NULL_POINTER on
 * NULL pointer.
 */
enum posix_times_error_code posix_times_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_TIMES_H */
