#ifndef POSIX_NETDB_RESOLV_H
#define POSIX_NETDB_RESOLV_H

/**
 * @file resolv.h
 * @brief Cross-platform <resolv.h> compatibility shim.
 */

/* clang-format off */
#if !defined(_WIN32)
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <resolv.h>
#else
#include <resolv.h>
#endif
#else
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#endif /* _WIN32 */
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)

/** @brief Maximum number of nameservers tracked in _res. */
#define MAXNS 3

/**
 * @brief Minimal BSD/POSIX resolver state structure for Windows.
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
 * @return Always returns 0 on Windows.
 */
static __inline int res_init(void) { return 0; }

/**
 * @brief Returns a pointer to the thread-local resolver state on Windows.
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

#endif /* _WIN32 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_NETDB_RESOLV_H */
