/* signal.h - Strict C89 Header */
#ifndef POSIX_SIGNAL_SIGNAL_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define POSIX_SIGNAL_SIGNAL_H

/**
 * @file signal.h
 * @brief Standard POSIX signal.h header wrapper for MSVC and others.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <signal.h>
#else
#include <signal.h>
#endif
#else
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/signal.h>
#else
#include <signal.h>
#endif
#include "posix-signal.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SIGNAL_SIGNAL_H */
