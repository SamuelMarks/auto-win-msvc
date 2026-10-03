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
