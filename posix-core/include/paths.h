#ifndef POSIX_CORE_PATHS_H
#define POSIX_CORE_PATHS_H

#if defined(__GNUC__)
#pragma GCC system_header
#endif

/* clang-format off */
#if !defined(_WIN32) && !defined(_MSC_VER)
#if defined(__GNUC__) || defined(__clang__)
#include_next <paths.h>
#else
#include <paths.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file paths.h
 * @brief Standard path definitions for POSIX environments and Windows polyfill.
 */

#if defined(_WIN32) || defined(_MSC_VER)

/** @brief Default search path. */
#ifndef _PATH_DEFPATH
#define _PATH_DEFPATH "C:\\Windows\\system32;C:\\Windows"
#endif

/** @brief Standard system path. */
#ifndef _PATH_STDPATH
#define _PATH_STDPATH "C:\\Windows\\system32;C:\\Windows"
#endif

/** @brief Path to the default shell executable. */
#ifndef _PATH_BSHELL
#define _PATH_BSHELL "dash.exe"
#endif

/** @brief Path to the null device. */
#ifndef _PATH_DEVNULL
#define _PATH_DEVNULL "NUL"
#endif

/** @brief Path to the controlling terminal. */
#ifndef _PATH_TTY
#define _PATH_TTY "CON"
#endif

/** @brief Path to the temporary directory. */
#ifndef _PATH_TMP
#define _PATH_TMP "C:\\Windows\\Temp\\"
#endif

#endif /* defined(_WIN32) || defined(_MSC_VER) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_CORE_PATHS_H */
