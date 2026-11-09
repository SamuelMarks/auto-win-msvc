#ifndef POSIX_POLL_POLL_H
#define POSIX_POLL_POLL_H

/**
 * @file poll.h
 * @brief POSIX poll.h redirection header.
 */

/* clang-format off */
#if !defined(_WIN32) && !defined(__WATCOMC__) && !defined(__MSDOS__) &&        \
    !defined(__DOS__)
#if !defined(__APPLE__)
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <poll.h>
#else
#include <poll.h>
#endif
#endif
#include "posix-poll.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_POLL_POLL_H */
/* API Contract symbols: __dependencies__ __include_next__ */
