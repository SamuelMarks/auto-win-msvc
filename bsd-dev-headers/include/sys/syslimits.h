#ifndef BSD_SYS_SYSLIMITS_H
#define BSD_SYS_SYSLIMITS_H

/**
 * @file syslimits.h
 * @brief BSD <sys/syslimits.h> wrapper forwarding to <limits.h> on Win32/DOS.
 */

/* clang-format off */
#if defined(_WIN32) || defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)
#include <limits.h>
#elif defined(__GNUC__) || defined(__clang__)
#if defined(__has_include_next)
#if __has_include_next(<sys/syslimits.h>)
#pragma GCC system_header
#include_next <sys/syslimits.h>
#else
#include <limits.h>
#endif
#else
#include <limits.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_SYSLIMITS_H */
