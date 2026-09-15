#ifndef BSD_SYS_PARAM_H
#define BSD_SYS_PARAM_H

/**
 * @file bsd-sys-param.h
 * @brief Polyfill for BSD <sys/param.h> macros and utilities.
 */

/* clang-format off */
#include <stddef.h>
#if defined(_MSC_VER) || defined(_WIN32) || defined(__WATCOMC__) ||                defined(__DOS__)
#include <stdlib.h>
#else
#include <sys/param.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAXPATHLEN
/** @brief Maximum length of a pathname. */
#define MAXPATHLEN 4096
#endif

#ifndef MIN
/** @brief Returns minimum of two numbers. */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
/** @brief Returns maximum of two numbers. */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef howmany
/** @brief Ceiling division of x by y. */
#define howmany(x, y) (((x) + ((y) - 1)) / (y))
#endif

#ifndef roundup
/** @brief Rounds x up to multiple of y. */
#define roundup(x, y) ((((x) + ((y) - 1)) / (y)) * (y))
#endif

#ifndef powerof2
/** @brief Returns non-zero if x is a power of 2. */
#define powerof2(x) ((((x) - 1) & (x)) == 0)
#endif

/**
 * @brief Error codes returned by bsd-sys-param functions.
 */
enum bsd_sys_param_error_code {
  /** @brief Successful operation. */
  BSD_SYS_PARAM_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  BSD_SYS_PARAM_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves the maximum path length defined in sys/param.h.
 * @param[out] out_maxlen Pointer to a size_t that receives the max path length.
 * @return BSD_SYS_PARAM_SUCCESS on success, or an error code on failure.
 */
enum bsd_sys_param_error_code bsd_sys_param_get_maxpathlen(size_t *out_maxlen);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_PARAM_H */
