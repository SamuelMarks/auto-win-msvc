#ifndef POSIX_LANGINFO_H
#define POSIX_LANGINFO_H

/**
 * @file posix-langinfo.h
 * @brief POSIX langinfo module interface and availability queries.
 */

/* clang-format off */
#include "langinfo.h"
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by posix-langinfo functions.
 */
enum posix_langinfo_error_code {
  /** @brief Operation completed successfully. */
  POSIX_LANGINFO_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_LANGINFO_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on posix-langinfo availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_LANGINFO_SUCCESS on success, or
 * POSIX_LANGINFO_ERROR_NULL_POINTER on NULL pointer.
 */
enum posix_langinfo_error_code posix_langinfo_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_LANGINFO_H */
