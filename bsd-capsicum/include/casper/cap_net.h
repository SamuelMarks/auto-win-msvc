#ifndef CASPER_CAP_NET_H
#define CASPER_CAP_NET_H

/**
 * @file cap_net.h
 * @brief FreeBSD Casper system.net service passthrough shim.
 */

/* clang-format off */
#include <libcasper.h>
#include <sys/capsicum.h>
#include <sys/socket.h>
#include <netdb.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Permit reverse DNS lookups. */
#define CAPNET_ADDR2NAME 0x01
/** @brief Permit forward DNS lookups. */
#define CAPNET_NAME2ADDR 0x02
/** @brief Permit deprecated gethostbyaddr lookups. */
#define CAPNET_DEPRECATED_ADDR2NAME 0x04
/** @brief Permit deprecated gethostbyname lookups. */
#define CAPNET_DEPRECATED_NAME2ADDR 0x08
/** @brief Permit connect() operations. */
#define CAPNET_CONNECT 0x10
/** @brief Permit bind() operations. */
#define CAPNET_BIND 0x20
/** @brief Permit connect() to resolved DNS addresses. */
#define CAPNET_CONNECTDNS 0x40

/** @brief Opaque Casper network limit handle. */
struct cap_net_limit;
/** @brief Typedef for Casper network limit handle. */
typedef struct cap_net_limit cap_net_limit_t;

/** @brief Bind socket via Casper net channel. */
static __inline int cap_bind(cap_channel_t *chan, int s,
                             const struct sockaddr *addr, socklen_t addrlen) {
  (void)chan;
  return bind(s, addr, addrlen);
}

/** @brief Connect socket via Casper net channel. */
static __inline int cap_connect(cap_channel_t *chan, int s,
                                const struct sockaddr *name,
                                socklen_t namelen) {
  (void)chan;
  return connect(s, name, namelen);
}

/** @brief Resolve host/service via Casper net channel. */
static __inline int cap_getaddrinfo(cap_channel_t *chan, const char *hostname,
                                    const char *servname,
                                    const struct addrinfo *hints,
                                    struct addrinfo **res) {
  (void)chan;
  return getaddrinfo(hostname, servname, hints, res);
}

/** @brief Reverse-resolve socket address via Casper net channel. */
static __inline int cap_getnameinfo(cap_channel_t *chan,
                                    const struct sockaddr *sa, socklen_t salen,
                                    char *host, size_t hostlen, char *serv,
                                    size_t servlen, int flags) {
  (void)chan;
  return getnameinfo(sa, salen, host, (socklen_t)hostlen, serv,
                     (socklen_t)servlen, flags);
}

/** @brief Look up host by name via Casper net channel. */
static __inline struct hostent *cap_gethostbyname(cap_channel_t *chan,
                                                  const char *name) {
  (void)chan;
  return gethostbyname(name);
}

/** @brief Look up host by name and address family via Casper net channel. */
static __inline struct hostent *cap_gethostbyname2(cap_channel_t *chan,
                                                   const char *name, int af) {
  (void)chan;
  (void)af;
  return gethostbyname(name);
}

/** @brief Look up host by address via Casper net channel. */
static __inline struct hostent *cap_gethostbyaddr(cap_channel_t *chan,
                                                  const void *addr,
                                                  socklen_t len, int af) {
  (void)chan;
  return gethostbyaddr((const char *)addr, len, af);
}

/** @brief Initialize Casper network limit builder. */
static __inline cap_net_limit_t *cap_net_limit_init(cap_channel_t *chan,
                                                    uint64_t mode) {
  (void)chan;
  (void)mode;
  return (cap_net_limit_t *)chan;
}

/** @brief Apply network limit configuration to Casper net channel. */
static __inline int cap_net_limit(cap_net_limit_t *limit) {
  (void)limit;
  return 0;
}

/** @brief Free network limit builder without applying. */
static __inline void cap_net_free(cap_net_limit_t *limit) { (void)limit; }

/** @brief Restrict reverse DNS lookups to given address families. */
static __inline cap_net_limit_t *
cap_net_limit_addr2name_family(cap_net_limit_t *limit, int *family,
                               size_t size) {
  (void)family;
  (void)size;
  return limit;
}

/** @brief Restrict reverse DNS lookups to specific socket address. */
static __inline cap_net_limit_t *
cap_net_limit_addr2name(cap_net_limit_t *limit, const struct sockaddr *sa,
                        socklen_t salen) {
  (void)sa;
  (void)salen;
  return limit;
}

/** @brief Restrict forward DNS lookups to given address families. */
static __inline cap_net_limit_t *
cap_net_limit_name2addr_family(cap_net_limit_t *limit, int *family,
                               size_t size) {
  (void)family;
  (void)size;
  return limit;
}

/** @brief Restrict forward DNS lookups to specific host and service. */
static __inline cap_net_limit_t *cap_net_limit_name2addr(cap_net_limit_t *limit,
                                                         const char *name,
                                                         const char *serv) {
  (void)name;
  (void)serv;
  return limit;
}

/** @brief Restrict connect() calls to specific socket address. */
static __inline cap_net_limit_t *
cap_net_limit_connect(cap_net_limit_t *limit, const struct sockaddr *sa,
                      socklen_t salen) {
  (void)sa;
  (void)salen;
  return limit;
}

/** @brief Restrict bind() calls to specific socket address. */
static __inline cap_net_limit_t *cap_net_limit_bind(cap_net_limit_t *limit,
                                                    const struct sockaddr *sa,
                                                    socklen_t salen) {
  (void)sa;
  (void)salen;
  return limit;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CASPER_CAP_NET_H */
