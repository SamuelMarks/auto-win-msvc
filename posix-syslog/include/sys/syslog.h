#ifndef POSIX_SYS_SYSLOG_H
#define POSIX_SYS_SYSLOG_H

/**
 * @file sys/syslog.h
 * @brief POSIX <sys/syslog.h> redirection header.
 */

/* clang-format off */
#if defined(_WIN32) || defined(__DOS__) || defined(__MSDOS__)
#include <syslog.h>
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/syslog.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_SYSLOG_H */
