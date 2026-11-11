#ifndef CASPER_CAP_NET_H
#define CASPER_CAP_NET_H

/**
 * @file cap_net.h
 * @brief FreeBSD Casper system.net service passthrough shim.
 */

/* clang-format off */
#include <libcasper.h>
#include <sys/socket.h>
#include <netdb.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define CAPNET_ADDR2NAME 0x01
#define CAPNET_NAME2ADDR 0x02
#define CAPNET_DEPRECATED_ADDR2NAME 0x04
#define CAPNET_DEPRECATED_NAME2ADDR 0x08
#define CAPNET_CONNECT 0x10
#define CAPNET_BIND 0x20
#define CAPNET_CONNECTDNS 0x40

/** @brief Opaque Casper network limit handle. */
struct cap_net_limit;
typedef struct cap_net_limit cap_net_limit_t;

static __inline int cap_bind(cap_channel_t *chan, int s,
                             const struct sockaddr *addr, socklen_t addrlen) {
  (void)chan;
  return bind(s, addr, addrlen);
}

static __inline int cap_connect(cap_channel_t *chan, int s,
                                const struct sockaddr *name,
                                socklen_t namelen) {
  (void)chan;
  return connect(s, name, namelen);
}

static __inline int cap_getaddrinfo(cap_channel_t *chan, const char *hostname,
                                    const char *servname,
                                    const struct addrinfo *hints,
                                    struct addrinfo **res) {
  (void)chan;
  return getaddrinfo(hostname, servname, hints, res);
}

static __inline int cap_getnameinfo(cap_channel_t *chan,
                                    const struct sockaddr *sa, socklen_t salen,
                                    char *host, size_t hostlen, char *serv,
                                    size_t servlen, int flags) {
  (void)chan;
  return getnameinfo(sa, salen, host, (socklen_t)hostlen, serv,
                     (socklen_t)servlen, flags);
}

static __inline struct hostent *cap_gethostbyname(cap_channel_t *chan,
                                                  const char *name) {
  (void)chan;
  return gethostbyname(name);
}

static __inline struct hostent *cap_gethostbyname2(cap_channel_t *chan,
                                                   const char *name, int af) {
  (void)chan;
  (void)af;
  return gethostbyname(name);
}

static __inline struct hostent *cap_gethostbyaddr(cap_channel_t *chan,
                                                  const void *addr,
                                                  socklen_t len, int af) {
  (void)chan;
  return gethostbyaddr((const char *)addr, len, af);
}

static __inline cap_net_limit_t *cap_net_limit_init(cap_channel_t *chan,
                                                    uint64_t mode) {
  (void)chan;
  (void)mode;
  return (cap_net_limit_t *)chan;
}

static __inline int cap_net_limit(cap_net_limit_t *limit) {
  (void)limit;
  return 0;
}

static __inline void cap_net_free(cap_net_limit_t *limit) { (void)limit; }

static __inline cap_net_limit_t *
cap_net_limit_addr2name_family(cap_net_limit_t *limit, int *family,
                               size_t size) {
  (void)family;
  (void)size;
  return limit;
}

static __inline cap_net_limit_t *
cap_net_limit_addr2name(cap_net_limit_t *limit, const struct sockaddr *sa,
                        socklen_t salen) {
  (void)sa;
  (void)salen;
  return limit;
}

static __inline cap_net_limit_t *
cap_net_limit_name2addr_family(cap_net_limit_t *limit, int *family,
                               size_t size) {
  (void)family;
  (void)size;
  return limit;
}

static __inline cap_net_limit_t *cap_net_limit_name2addr(cap_net_limit_t *limit,
                                                         const char *name,
                                                         const char *serv) {
  (void)name;
  (void)serv;
  return limit;
}

static __inline cap_net_limit_t *
cap_net_limit_connect(cap_net_limit_t *limit, const struct sockaddr *sa,
                      socklen_t salen) {
  (void)sa;
  (void)salen;
  return limit;
}

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
