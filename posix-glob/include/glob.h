#ifndef POSIX_GLOB_GLOB_H
#if defined(__GNUC__)
#endif
#define POSIX_GLOB_GLOB_H

/**
 * @file glob.h
 * @brief POSIX glob.h redirection header.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <glob.h>
#else
#include <glob.h>
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

#endif /* POSIX_GLOB_GLOB_H */
