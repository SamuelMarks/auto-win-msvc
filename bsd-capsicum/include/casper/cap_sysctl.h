#ifndef CASPER_CAP_SYSCTL_H
#define CASPER_CAP_SYSCTL_H

/**
 * @file cap_sysctl.h
 * @brief FreeBSD Casper system.sysctl service passthrough shim.
 */

/* clang-format off */
#include <libcasper.h>
#include <sys/sysctl.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Allow reading sysctl variable. */
#define CAP_SYSCTL_READ 0x01
/** @brief Allow writing sysctl variable. */
#define CAP_SYSCTL_WRITE 0x02
/** @brief Allow reading and writing sysctl variable. */
#define CAP_SYSCTL_RDWR (CAP_SYSCTL_READ | CAP_SYSCTL_WRITE)
/** @brief Apply sysctl limit recursively to subtree. */
#define CAP_SYSCTL_RECURSIVE 0x04

/** @brief Opaque Casper sysctl limit handle. */
typedef void cap_sysctl_limit_t;

/** @brief Query or set sysctl MIB variable via Casper channel. */
static __inline int cap_sysctl(cap_channel_t *chan, const int *name,
                               unsigned int namelen, void *oldp,
                               size_t *oldlenp, const void *newp,
                               size_t newlen) {
  (void)chan;
#if defined(__APPLE__)
  {
    union {
      const int *ci;
      int *mi;
    } un;
    union {
      const void *cv;
      void *mv;
    } uw;
    un.ci = name;
    uw.cv = newp;
    return sysctl(un.mi, namelen, oldp, oldlenp, uw.mv, newlen);
  }
#else
  return sysctl(name, namelen, oldp, oldlenp, newp, newlen);
#endif
}

/** @brief Query or set sysctl variable by name via Casper channel. */
static __inline int cap_sysctlbyname(cap_channel_t *chan, const char *name,
                                     void *oldp, size_t *oldlenp,
                                     const void *newp, size_t newlen) {
  (void)chan;
#if defined(__APPLE__)
  {
    union {
      const void *cv;
      void *mv;
    } uw;
    uw.cv = newp;
    return sysctlbyname(name, oldp, oldlenp, uw.mv, newlen);
  }
#else
  return sysctlbyname(name, oldp, oldlenp, newp, newlen);
#endif
}

/** @brief Translate sysctl variable name to MIB array via Casper channel. */
static __inline int cap_sysctlnametomib(cap_channel_t *chan, const char *name,
                                        int *mibp, size_t *sizep) {
  (void)chan;
  return sysctlnametomib(name, mibp, sizep);
}

/** @brief Initialize Casper sysctl limit builder. */
static __inline cap_sysctl_limit_t *cap_sysctl_limit_init(cap_channel_t *chan) {
  return (cap_sysctl_limit_t *)chan;
}

/** @brief Add named sysctl variable constraint to limit builder. */
static __inline cap_sysctl_limit_t *
cap_sysctl_limit_name(cap_sysctl_limit_t *limit, const char *name, int flags) {
  (void)name;
  (void)flags;
  return limit;
}

/** @brief Add MIB sysctl variable constraint to limit builder. */
static __inline cap_sysctl_limit_t *
cap_sysctl_limit_mib(cap_sysctl_limit_t *limit, const int *mibp,
                     unsigned int miblen, int flags) {
  (void)mibp;
  (void)miblen;
  (void)flags;
  return limit;
}

/** @brief Apply accumulated sysctl limits to Casper channel. */
static __inline int cap_sysctl_limit(cap_sysctl_limit_t *limit) {
  (void)limit;
  return 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CASPER_CAP_SYSCTL_H */
