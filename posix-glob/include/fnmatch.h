#ifndef POSIX_GLOB_FNMATCH_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define POSIX_GLOB_FNMATCH_H

/**
 * @file fnmatch.h
 * @brief POSIX fnmatch.h redirection header.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <fnmatch.h>
#else
#include <fnmatch.h>
#endif
#else
#include "posix-glob.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_GLOB_FNMATCH_H */
