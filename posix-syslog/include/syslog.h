#ifndef POSIX_SYSLOG_SYSLOG_H
#define POSIX_SYSLOG_SYSLOG_H

/**
 * @file syslog.h
 * @brief POSIX syslog.h redirection header with BSD SYSLOG_NAMES support.
 */

/* clang-format off */
#if !defined(_WIN32) && !defined(__WATCOMC__) && !defined(__MSDOS__) &&        \
    !defined(__DOS__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
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

#if defined(SYSLOG_NAMES) && !defined(POSIX_SYSLOG_NAMES_EMITTED) &&           \
    !defined(INTERNAL_NOPRI)
#define POSIX_SYSLOG_NAMES_EMITTED 1
#define INTERNAL_NOPRI 0x10
#define INTERNAL_MARK (LOG_NFACILITIES << 3)
#ifndef LOG_NFACILITIES
#define LOG_NFACILITIES 24
#endif
#ifndef LOG_AUTHPRIV
#define LOG_AUTHPRIV (10 << 3)
#endif
#ifndef LOG_FTP
#define LOG_FTP (11 << 3)
#endif
#ifndef LOG_NTP
#define LOG_NTP (12 << 3)
#endif
#ifndef LOG_SECURITY
#define LOG_SECURITY (13 << 3)
#endif
#ifndef LOG_CONSOLE
#define LOG_CONSOLE (14 << 3)
#endif
typedef struct _awm_syslog_code {
  const char *c_name;
  int c_val;
} CODE;

static CODE prioritynames[] = {{"alert", LOG_ALERT},
                               {"crit", LOG_CRIT},
                               {"debug", LOG_DEBUG},
                               {"emerg", LOG_EMERG},
                               {"err", LOG_ERR},
                               {"error", LOG_ERR},
                               {"info", LOG_INFO},
                               {"none", INTERNAL_NOPRI},
                               {"notice", LOG_NOTICE},
                               {"panic", LOG_EMERG},
                               {"warn", LOG_WARNING},
                               {"warning", LOG_WARNING},
                               {0, -1}};

static CODE facilitynames[] = {{"auth", LOG_AUTH},
                               {"authpriv", LOG_AUTHPRIV},
                               {"console", LOG_CONSOLE},
                               {"cron", LOG_CRON},
                               {"daemon", LOG_DAEMON},
                               {"ftp", LOG_FTP},
                               {"kern", LOG_KERN},
                               {"lpr", LOG_LPR},
                               {"mail", LOG_MAIL},
                               {"mark", INTERNAL_MARK},
                               {"news", LOG_NEWS},
                               {"ntp", LOG_NTP},
                               {"security", LOG_SECURITY},
                               {"syslog", LOG_SYSLOG},
                               {"user", LOG_USER},
                               {"uucp", LOG_UUCP},
                               {"local0", LOG_LOCAL0},
                               {"local1", LOG_LOCAL1},
                               {"local2", LOG_LOCAL2},
                               {"local3", LOG_LOCAL3},
                               {"local4", LOG_LOCAL4},
                               {"local5", LOG_LOCAL5},
                               {"local6", LOG_LOCAL6},
                               {"local7", LOG_LOCAL7},
                               {0, -1}};
#endif
