#ifndef POSIX_STDBOOL_H
#define POSIX_STDBOOL_H

/**
 * @file posix-stdbool.h
 * @brief Polyfill for C99 <stdbool.h> boolean types and macros.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _STDBOOL
#define _STDBOOL

/** @brief Macro defining whether bool, true, and false are defined. */
#define __bool_true_false_are_defined 1

#ifndef __cplusplus

#if defined(_MSC_VER) && (_MSC_VER < 1800)
#define bool unsigned char
#elif defined(_MSC_VER)
#define bool _Bool
#elif defined(__GNUC__) || defined(__clang__)
#define bool unsigned char
#else
#define bool unsigned char
#endif

#ifndef true
/** @brief Boolean true value. */
#define true 1
#endif
#ifndef false
/** @brief Boolean false value. */
#define false 0
#endif

#endif /* !__cplusplus */

#endif /* _STDBOOL */

/**
 * @brief Error codes returned by posix-stdbool functions.
 */
enum posix_stdbool_error_code {
  /** @brief Operation completed successfully. */
  POSIX_STDBOOL_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_STDBOOL_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on stdbool polyfill availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_STDBOOL_SUCCESS on success, or an error code on failure.
 */
enum posix_stdbool_error_code posix_stdbool_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_STDBOOL_H */
