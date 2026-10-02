#ifndef POSIX_POLL_POLL_H
#if defined(__GNUC__)
#endif
#define POSIX_POLL_POLL_H

/**
 * @file poll.h
 * @brief POSIX poll.h redirection header.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <poll.h>
#else
#include <poll.h>
#endif
#else
#include "posix-poll.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_POLL_POLL_H */

/* API Contract symbols: __dependencies__, __include_next__ */
