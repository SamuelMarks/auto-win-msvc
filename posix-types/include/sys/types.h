/* sys/types.h - Strict C89 Header */
#ifndef SYS_TYPES_WRAPPER_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define SYS_TYPES_WRAPPER_H

/**
 * @file types.h
 * @brief Standard POSIX sys/types.h header wrapper for MSVC.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <sys/types.h>
#else
#include <sys/types.h>
#endif
#else
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/sys/types.h>
#elif defined(_MSC_VER)
#include <../include/sys/types.h>
#elif defined(__GNUC__) || defined(__clang__)
#include_next <sys/types.h>
#else
#include <sys/types.h>
#endif
#include "posix-types.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_TYPES_WRAPPER_H */
