#ifndef CASPER_CAP_NETDB_H
#define CASPER_CAP_NETDB_H

/**
 * @file cap_netdb.h
 * @brief FreeBSD Casper system.netdb service passthrough shim.
 */

/* clang-format off */
#include <libcasper.h>
#include <netdb.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

static __inline struct protoent *cap_getprotobyname(cap_channel_t *chan,
                                                    const char *name) {
  (void)chan;
  return getprotobyname(name);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CASPER_CAP_NETDB_H */
