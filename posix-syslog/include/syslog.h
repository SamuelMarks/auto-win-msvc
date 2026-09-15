#ifndef POSIX_SYSLOG_SYSLOG_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define POSIX_SYSLOG_SYSLOG_H

/**
 * @file syslog.h
 * @brief POSIX syslog.h redirection header.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <syslog.h>
#else
#include <syslog.h>
#endif
#else
#include "posix-syslog.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYSLOG_SYSLOG_H */
