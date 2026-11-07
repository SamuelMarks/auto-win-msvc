#ifndef POSIX_NETINET_IN_H
#define POSIX_NETINET_IN_H

/**
 * @file posix-netinet-in.h
 * @brief POSIX netinet/in.h compatibility layer for MSVC.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(__MSDOS__) || defined(__WATCOMC__)
#include <stddef.h>
#else
#include <netinet/in.h>
#endif
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IPPROTO_IP
/** @brief Dummy IP protocol for IPv4. */
#define IPPROTO_IP 0
#endif
#ifndef IPPROTO_TCP
/** @brief Transmission Control Protocol. */
#define IPPROTO_TCP 6
#endif
#ifndef IPPROTO_UDP
/** @brief User Datagram Protocol. */
#define IPPROTO_UDP 17
#endif
#ifndef INADDR_ANY
/** @brief Bind to any incoming address. */
#define INADDR_ANY ((unsigned long)0x00000000)
#endif
#ifndef INADDR_LOOPBACK
/** @brief Localhost IPv4 address. */
#define INADDR_LOOPBACK ((unsigned long)0x7f000001)
#endif

#if defined(__MSDOS__) || defined(__WATCOMC__) || defined(__DOS__)
#ifndef _IN_ADDR_DEFINED
#define _IN_ADDR_DEFINED
struct in_addr {
  unsigned long s_addr;
};
#endif

#ifndef _SOCKADDR_IN_DEFINED
#define _SOCKADDR_IN_DEFINED
struct sockaddr_in {
  short sin_family;
  unsigned short sin_port;
  struct in_addr sin_addr;
  char sin_zero[8];
};
#endif

#ifndef _IN6_ADDR_DEFINED
#define _IN6_ADDR_DEFINED
struct in6_addr {
  unsigned char s6_addr[16];
};
#endif

#ifndef _SOCKADDR_IN6_DEFINED
#define _SOCKADDR_IN6_DEFINED
struct sockaddr_in6 {
  short sin6_family;
  unsigned short sin6_port;
  unsigned long sin6_flowinfo;
  struct in6_addr sin6_addr;
  unsigned long sin6_scope_id;
};
#endif

#ifndef htonl
#define htonl(x)                                                               \
  ((((unsigned long)(x) & 0xff000000UL) >> 24) |                               \
   (((unsigned long)(x) & 0x00ff0000UL) >> 8) |                                \
   (((unsigned long)(x) & 0x0000ff00UL) << 8) |                                \
   (((unsigned long)(x) & 0x000000ffUL) << 24))
#endif
#ifndef ntohl
#define ntohl(x) htonl(x)
#endif
#ifndef htons
#define htons(x)                                                               \
  ((unsigned short)((((unsigned short)(x) & 0xff00U) >> 8) |                   \
                    (((unsigned short)(x) & 0x00ffU) << 8)))
#endif
#ifndef ntohs
#define ntohs(x) htons(x)
#endif
#endif

/**
 * @brief Retrieves information on posix-netinet availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_netinet_get_info(int *out_available);

/**
 * @brief Checks if an IPv4 address (in host byte order) is in the loopback
 * range (127.0.0.0/8).
 * @param[in] addr IPv4 address in host byte order.
 * @param[out] out_is_loopback Pointer to integer receiving 1 if loopback, 0
 * otherwise.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_netinet_is_ipv4_loopback(unsigned long addr,
                                                     int *out_is_loopback);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_NETINET_IN_H */
