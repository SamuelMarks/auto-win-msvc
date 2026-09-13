#if defined(__GNUC__)
#pragma GCC system_header
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
/* clang-format off */
#include_next <strings.h>
#else
#include <strings.h>
#endif
#else
#include "posix-strings.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) || defined(_WIN32)
#ifndef strtok_r
/** \brief Thread-safe strtok_r mapping to strtok_s on Windows/MSVC. */
#define strtok_r strtok_s
#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
