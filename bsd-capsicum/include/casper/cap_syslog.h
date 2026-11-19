#ifndef CASPER_CAP_SYSLOG_H
#define CASPER_CAP_SYSLOG_H

/**
 * @file cap_syslog.h
 * @brief FreeBSD Casper system.syslog service passthrough shim.
 */

/* clang-format off */
#include <libcasper.h>
#include <syslog.h>
#include <stdarg.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) || defined(__clang__)
/** @brief Compiler attribute for printf-style format checking in cap_syslog. */
#define CAP_SYSLOG_PRINTF(fmt_idx, arg_idx)                                    \
  __attribute__((__format__(__printf__, fmt_idx, arg_idx)))
#else
/** @brief Compiler attribute for printf-style format checking in cap_syslog. */
#define CAP_SYSLOG_PRINTF(fmt_idx, arg_idx)
#endif

static __inline void cap_vsyslog(cap_channel_t *chan, int priority,
                                 const char *message, va_list args)
    CAP_SYSLOG_PRINTF(3, 0);
static __inline void cap_syslog(cap_channel_t *chan, int priority,
                                const char *message, ...)
    CAP_SYSLOG_PRINTF(3, 4);

/** @brief Log formatted message via va_list over Casper syslog channel. */
static __inline void cap_vsyslog(cap_channel_t *chan, int priority,
                                 const char *message, va_list args) {
  (void)chan;
  vsyslog(priority, message, args);
}

/** @brief Log formatted message over Casper syslog channel. */
static __inline void cap_syslog(cap_channel_t *chan, int priority,
                                const char *message, ...) {
  va_list ap;
  va_start(ap, message);
  cap_vsyslog(chan, priority, message, ap);
  va_end(ap);
}

/** @brief Open system log connection via Casper syslog channel. */
static __inline void cap_openlog(cap_channel_t *chan, const char *ident,
                                 int logopt, int facility) {
  (void)chan;
  openlog(ident, logopt, facility);
}

/** @brief Close system log connection via Casper syslog channel. */
static __inline void cap_closelog(cap_channel_t *chan) {
  (void)chan;
  closelog();
}

/** @brief Set syslog priority mask via Casper syslog channel. */
static __inline int cap_setlogmask(cap_channel_t *chan, int maskpri) {
  (void)chan;
  return setlogmask(maskpri);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CASPER_CAP_SYSLOG_H */
