#ifndef POSIX_ALLOCA_H
#define POSIX_ALLOCA_H

/**
 * @file alloca.h
 * @brief Strict C89 polyfill and wrapper for alloca.
 */

/* clang-format off */
#if defined(_MSC_VER) || defined(__WATCOMC__) || defined(__MSDOS__) || defined(__MINGW32__)
#include <malloc.h>
#else
#include <stdlib.h>
#endif
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) && !defined(alloca)
/** @brief Macro mapping alloca to MSVC _alloca. */
#define alloca _alloca
#elif (defined(__GNUC__) || defined(__clang__)) && !defined(alloca)
/** @brief Macro mapping alloca to GCC/Clang built-in alloca. */
#define alloca __builtin_alloca
#endif

/**
 * @brief Error codes returned by posix-alloca functions.
 */
enum posix_alloca_error_code {
  /** @brief Operation completed successfully. */
  POSIX_ALLOCA_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_ALLOCA_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on alloca availability.
 * @param[out] out_available Pointer to an integer set to 1 if alloca is
 * available.
 * @return POSIX_ALLOCA_SUCCESS on success, or an error code on failure.
 */
enum posix_alloca_error_code posix_alloca_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_ALLOCA_H */
