#ifndef POSIX_TIME_UTIME_H
#define POSIX_TIME_UTIME_H

/**
 * @file utime.h
 * @brief POSIX <utime.h> wrapper header.
 */

/* clang-format off */
#if !defined(_WIN32) && !defined(__WATCOMC__) && !defined(__MSDOS__) &&        \
    !defined(__DOS__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <utime.h>
#else
#include <utime.h>
#endif
#elif defined(_MSC_VER)
#include <sys/utime.h>
#elif defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)
#include <sys/utime.h>
#else
#include <sys/utime.h>
#endif
#include "posix-time.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_TIME_UTIME_H */
