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

static __inline void cap_vsyslog(cap_channel_t *chan, int priority,
                                 const char *message, va_list args) {
  (void)chan;
  vsyslog(priority, message, args);
}

static __inline void cap_syslog(cap_channel_t *chan, int priority,
                                const char *message, ...) {
  va_list ap;
  va_start(ap, message);
  cap_vsyslog(chan, priority, message, ap);
  va_end(ap);
}

static __inline void cap_openlog(cap_channel_t *chan, const char *ident,
                                 int logopt, int facility) {
  (void)chan;
  openlog(ident, logopt, facility);
}

static __inline void cap_closelog(cap_channel_t *chan) {
  (void)chan;
  closelog();
}

static __inline int cap_setlogmask(cap_channel_t *chan, int maskpri) {
  (void)chan;
  return setlogmask(maskpri);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CASPER_CAP_SYSLOG_H */
