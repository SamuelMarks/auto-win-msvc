#ifndef POSIX_STRINGS_STRINGS_H
#define POSIX_STRINGS_STRINGS_H

/**
 * @file strings.h
 * @brief Compatibility header providing <strings.h> on Windows / MSVC.
 */

#if defined(__GNUC__)
#pragma GCC system_header
#endif

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <strings.h>
#else
#include <strings.h>
#endif
#else
#include "posix-strings.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) || defined(_WIN32)
#ifndef strtok_r
/** @brief Thread-safe strtok_r mapping to strtok_s on Windows/MSVC. */
#define strtok_r strtok_s
#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_STRINGS_STRINGS_H */
