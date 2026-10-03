#ifndef BSD_SYS_PARAM_H
#define BSD_SYS_PARAM_H

/**
 * @file bsd-sys-param.h
 * @brief Polyfill for BSD <sys/param.h> macros and utilities.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
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

#ifndef PIPE_BUF
/** @brief Maximum number of bytes guaranteed to be written atomically to a
 * pipe. */
#define PIPE_BUF 4096
#endif

#ifndef BSD4_4
/** @brief BSD 4.4 compatibility level identifier. */
#define BSD4_4 1
#endif

#ifndef NCARGS
/** @brief Maximum length of arguments and environment to execve. */
#define NCARGS 262144
#endif

#ifndef NOFILE
/** @brief Maximum number of open files per process. */
#define NOFILE 2048
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
 * @brief Retrieves the maximum path length defined in sys/param.h.
 * @param[out] out_maxlen Pointer to a size_t that receives the max path length.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t bsd_sys_param_get_maxpathlen(size_t *out_maxlen);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_PARAM_H */
