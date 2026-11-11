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

#define CAP_SYSCTL_READ 0x01
#define CAP_SYSCTL_WRITE 0x02
#define CAP_SYSCTL_RDWR (CAP_SYSCTL_READ | CAP_SYSCTL_WRITE)
#define CAP_SYSCTL_RECURSIVE 0x04

typedef void cap_sysctl_limit_t;

static __inline int cap_sysctl(cap_channel_t *chan, const int *name,
                               unsigned int namelen, void *oldp,
                               size_t *oldlenp, const void *newp,
                               size_t newlen) {
  (void)chan;
  return sysctl((int *)(uintptr_t)name, namelen, oldp, oldlenp,
                (void *)(uintptr_t)newp, newlen);
}

static __inline int cap_sysctlbyname(cap_channel_t *chan, const char *name,
                                     void *oldp, size_t *oldlenp,
                                     const void *newp, size_t newlen) {
  (void)chan;
  return sysctlbyname(name, oldp, oldlenp, (void *)(uintptr_t)newp, newlen);
}

static __inline int cap_sysctlnametomib(cap_channel_t *chan, const char *name,
                                        int *mibp, size_t *sizep) {
  (void)chan;
  return sysctlnametomib(name, mibp, sizep);
}

static __inline cap_sysctl_limit_t *cap_sysctl_limit_init(cap_channel_t *chan) {
  return (cap_sysctl_limit_t *)chan;
}

static __inline cap_sysctl_limit_t *
cap_sysctl_limit_name(cap_sysctl_limit_t *limit, const char *name, int flags) {
  (void)name;
  (void)flags;
  return limit;
}

static __inline cap_sysctl_limit_t *
cap_sysctl_limit_mib(cap_sysctl_limit_t *limit, const int *mibp,
                     unsigned int miblen, int flags) {
  (void)mibp;
  (void)miblen;
  (void)flags;
  return limit;
}

static __inline int cap_sysctl_limit(cap_sysctl_limit_t *limit) {
  (void)limit;
  return 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CASPER_CAP_SYSCTL_H */
