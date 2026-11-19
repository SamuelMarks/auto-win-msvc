#ifndef POSIX_NETDB_POSIX_RESOLV_H
#define POSIX_NETDB_POSIX_RESOLV_H

/**
 * @file posix-resolv.h
 * @brief Cross-platform BSD/POSIX resolver (<resolv.h>) definitions for Windows
 * and DOS.
 */

/* clang-format off */
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(__WATCOMC__) || defined(__MSDOS__) ||           \
    defined(__DOS__)

#ifndef MAXNS
/** @brief Maximum number of nameservers tracked in _res. */
#define MAXNS 3
#endif

#ifndef RES_INIT
/** @brief Resolver initialized option flag. */
#define RES_INIT 0x00000001UL
#endif

#ifndef RES_DEFNAMES
/** @brief Append default domain name to single-component names. */
#define RES_DEFNAMES 0x00000080UL
#endif

#ifndef RES_DNSRCH
/** @brief Search up local domain tree. */
#define RES_DNSRCH 0x00000200UL
#endif

#ifndef RES_USE_INET6
/** @brief Map IPv4 responses into IPv6 form where requested. */
#define RES_USE_INET6 0x00002000UL
#endif

#ifndef RES_DEFAULT
/** @brief Default resolver option bitmask. */
#define RES_DEFAULT (RES_INIT | RES_DEFNAMES | RES_DNSRCH)
#endif

/**
 * @brief Minimal BSD/POSIX resolver state structure for Windows and DOS.
 */
struct __res_state {
  int retrans;                           /**< Retransmission time interval */
  int retry;                             /**< Number of times to retransmit */
  unsigned long options;                 /**< Option flags */
  int nscount;                           /**< Number of name servers */
  struct sockaddr_in nsaddr_list[MAXNS]; /**< Address of name server */
};

/**
 * @brief Initializes the resolver state.
 * @return Always returns 0 on Windows and DOS.
 */
static __inline int res_init(void) { return 0; }

/**
 * @brief Returns a pointer to the resolver state instance.
 * @return Pointer to static __res_state instance.
 */
static __inline struct __res_state *posix_res_state_ptr(void) {
  static struct __res_state s_res;
  return &s_res;
}

#ifndef _res
/** @brief Global resolver state macro. */
#define _res (*posix_res_state_ptr())
#endif

#endif /* _WIN32 || __WATCOMC__ || __MSDOS__ || __DOS__ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_NETDB_POSIX_RESOLV_H */
