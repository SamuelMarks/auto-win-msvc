#ifndef POSIX_SYSLOG_H
#define POSIX_SYSLOG_H

/**
 * @file posix-syslog.h
 * @brief POSIX syslog.h compatibility layer for MSVC.
 */

/* clang-format off */
#include <stdarg.h>
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief System is unusable. */
#define LOG_EMERG 0
/** @brief Action must be taken immediately. */
#define LOG_ALERT 1
/** @brief Critical conditions. */
#define LOG_CRIT 2
/** @brief Error conditions. */
#define LOG_ERR 3
/** @brief Warning conditions. */
#define LOG_WARNING 4
/** @brief Normal but significant condition. */
#define LOG_NOTICE 5
/** @brief Informational message. */
#define LOG_INFO 6
/** @brief Debug-level message. */
#define LOG_DEBUG 7

/** @brief Kernel messages facility. */
#define LOG_KERN (0 << 3)
/** @brief Random user-level messages facility. */
#define LOG_USER (1 << 3)
/** @brief Mail system facility. */
#define LOG_MAIL (2 << 3)
/** @brief System daemons facility. */
#define LOG_DAEMON (3 << 3)
/** @brief Security/authorization messages facility. */
#define LOG_AUTH (4 << 3)
/** @brief Syslog internal messages facility. */
#define LOG_SYSLOG (5 << 3)
/** @brief Line printer subsystem facility. */
#define LOG_LPR (6 << 3)
/** @brief Network news subsystem facility. */
#define LOG_NEWS (7 << 3)
/** @brief UUCP subsystem facility. */
#define LOG_UUCP (8 << 3)
/** @brief Clock daemon facility. */
#define LOG_CRON (9 << 3)
/** @brief Private security/authorization messages facility. */
#define LOG_AUTHPRIV (10 << 3)
/** @brief FTP daemon facility. */
#define LOG_FTP (11 << 3)
/** @brief Reserved for local use 0. */
#define LOG_LOCAL0 (16 << 3)
/** @brief Reserved for local use 1. */
#define LOG_LOCAL1 (17 << 3)
/** @brief Reserved for local use 2. */
#define LOG_LOCAL2 (18 << 3)
/** @brief Reserved for local use 3. */
#define LOG_LOCAL3 (19 << 3)
/** @brief Reserved for local use 4. */
#define LOG_LOCAL4 (20 << 3)
/** @brief Reserved for local use 5. */
#define LOG_LOCAL5 (21 << 3)
/** @brief Reserved for local use 6. */
#define LOG_LOCAL6 (22 << 3)
/** @brief Reserved for local use 7. */
#define LOG_LOCAL7 (23 << 3)

/** @brief Log the PID with each message. */
#define LOG_PID 0x01
/** @brief Log on the console if errors in sending. */
#define LOG_CONS 0x02
/** @brief Delay open until first syslog(). */
#define LOG_ODELAY 0x04
/** @brief Don't delay open. */
#define LOG_NDELAY 0x08
/** @brief Don't wait for console forks (deprecated). */
#define LOG_NOWAIT 0x10
/** @brief Log to stderr as well. */
#define LOG_PERROR 0x20

/** @brief Combines facility and priority into a single priority value. */
#define LOG_MAKEPRI(fac, pri) ((fac) | (pri))
/** @brief Extracts the priority from a priority/facility value. */
#define LOG_PRI(p) ((p) & 7)
/** @brief Extracts the facility from a priority/facility value. */
#define LOG_FAC(p) (((p) & 0x03f8) >> 3)
/** @brief Creates a bit mask for a given priority. */
#define LOG_MASK(pri) (1 << (pri))
/** @brief Creates a mask for all priorities up to and including pri. */
#define LOG_UPTO(pri) ((1 << ((pri) + 1)) - 1)

/**
 * @brief Error codes returned by posix-syslog functions.
 */
enum posix_syslog_error_code {
  /** @brief Operation completed successfully. */
  POSIX_SYSLOG_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_SYSLOG_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on posix-syslog availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_SYSLOG_SUCCESS on success, or POSIX_SYSLOG_ERROR_NULL_POINTER
 * on NULL pointer.
 */
enum posix_syslog_error_code posix_syslog_get_info(int *out_available);

/**
 * @brief Closes the descriptor being used to write to the system logger.
 */
void closelog(void);

/**
 * @brief Opens a connection to the system logger.
 * @param ident The string to prepend to every message.
 * @param option Logging options (e.g., LOG_PID, LOG_NDELAY).
 * @param facility The default facility to assign to messages.
 */
void openlog(const char *ident, int option, int facility);

/**
 * @brief Sets the log priority mask.
 * @param mask The new log priority mask.
 * @return The previous log priority mask.
 */
int setlogmask(int mask);

#if defined(__GNUC__) || defined(__clang__)
#define POSIX_SYSLOG_PRINTF_ATTR(fmt, args)                                    \
  __attribute__((__format__(__printf__, fmt, args)))
#else
#define POSIX_SYSLOG_PRINTF_ATTR(fmt, args)
#endif

/**
 * @brief Generates a log message.
 * @param priority The priority and facility of the message.
 * @param format Format string.
 */
void syslog(int priority, const char *format, ...)
    POSIX_SYSLOG_PRINTF_ATTR(2, 3);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYSLOG_H */
