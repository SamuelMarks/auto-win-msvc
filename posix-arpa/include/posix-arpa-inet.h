#ifndef POSIX_ARPA_INET_H
#define POSIX_ARPA_INET_H

/**
 * @file posix-arpa-inet.h
 * @brief POSIX arpa/inet.h implementation and polyfills for MSVC.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <stddef.h>
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(__MSDOS__) || defined(__WATCOMC__) || defined(__DOS__)
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
#ifndef _IN_ADDR_DEFINED
#define _IN_ADDR_DEFINED
struct in_addr {
  unsigned long s_addr;
};
#endif
#ifndef htonl
#define htonl(x) ((((x) & 0xff000000UL) >> 24) | (((x) & 0x00ff0000UL) >> 8) | (((x) & 0x0000ff00UL) << 8) | (((x) & 0x000000ffUL) << 24))
#endif
#ifndef ntohl
#define ntohl(x) htonl(x)
#endif
#ifndef htons
#define htons(x) ((unsigned short)((((unsigned short)(x) & 0xff00U) >> 8) | (((unsigned short)(x) & 0x00ffU) << 8)))
#endif
#ifndef ntohs
#define ntohs(x) htons(x)
#endif
#else
#include <sys/types.h>
#include <netinet/in.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef INADDR_NONE
/** @brief Constant indicating an invalid IP address. */
#define INADDR_NONE 0xffffffffUL
#endif

/**
 * @brief Parses an IPv4 address in numbers-and-dots notation into binary form.
 * @param cp The input IP address string.
 * @param[out] out_addr Pointer to struct in_addr receiving the parsed binary
 * address.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t posix_arpa_inet_parse_ipv4(const char *cp,
                                                 struct in_addr *out_addr);

/**
 * @brief Converts IPv4 numbers-and-dots notation into binary form in network
 * byte order.
 * @param cp The input IP address string.
 * @param[out] inp Pointer to struct in_addr where the result is stored.
 * @return 1 if the address is valid, or 0 if invalid.
 */
int posix_inet_aton(const char *cp, struct in_addr *inp);

/**
 * @brief Converts IPv4 numbers-and-dots notation into binary data in network
 * byte order.
 * @param cp The input IP address string.
 * @return The IP address in network byte order, or INADDR_NONE if invalid.
 */
unsigned long posix_inet_addr(const char *cp);

#if defined(__MSDOS__) || defined(__WATCOMC__) || defined(__DOS__)
/** @brief Convert Internet host address to numbers-and-dots string. */
char *posix_inet_ntoa(struct in_addr in);
/** @brief Convert IPv4 and IPv6 addresses from text to binary form. */
int posix_inet_pton(int af, const char *src, void *dst);
/** @brief Convert IPv4 and IPv6 addresses from binary to text form. */
const char *posix_inet_ntop(int af, const void *src, char *dst, size_t size);
#ifndef inet_ntoa
/** @brief Macro mapping inet_ntoa to posix_inet_ntoa on DOS. */
#define inet_ntoa posix_inet_ntoa
#endif
#ifndef inet_pton
/** @brief Macro mapping inet_pton to posix_inet_pton on DOS. */
#define inet_pton posix_inet_pton
#endif
#ifndef inet_ntop
/** @brief Macro mapping inet_ntop to posix_inet_ntop on DOS. */
#define inet_ntop posix_inet_ntop
#endif
#endif

#if defined(_MSC_VER) || defined(_WIN32) || defined(__MSDOS__) ||              \
    defined(__WATCOMC__) || defined(__DOS__)
#undef inet_aton
/** @brief Macro mapping inet_aton to posix_inet_aton on Windows/DOS. */
#define inet_aton posix_inet_aton

#ifndef inet_addr
/** @brief Macro mapping inet_addr to posix_inet_addr on Windows/DOS. */
#define inet_addr posix_inet_addr
#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_ARPA_INET_H */
