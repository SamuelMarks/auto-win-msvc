/* netinet/ip.h - Strict C89 Header */
#ifndef POSIX_NETINET_IP_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define POSIX_NETINET_IP_H

/**
 * @file ip.h
 * @brief POSIX netinet/ip.h implementation for MSVC
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <netinet/ip.h>
#else
#include <netinet/ip.h>
#endif
#else
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../posix-netinet-in.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_MSC_VER)

#ifndef IPTOS_LOWDELAY
/** \brief Minimize delay. */
#define IPTOS_LOWDELAY 0x10
#endif
#ifndef IPTOS_THROUGHPUT
/** \brief Maximize throughput. */
#define IPTOS_THROUGHPUT 0x08
#endif
#ifndef IPTOS_RELIABILITY
/** \brief Maximize reliability. */
#define IPTOS_RELIABILITY 0x04
#endif
#ifndef IPTOS_MINCOST
/** \brief Minimize monetary cost. */
#define IPTOS_MINCOST 0x02
#endif

#ifndef IPVERSION
/** \brief IP version number. */
#define IPVERSION 4
#endif

#endif /* defined(_WIN32) || defined(_MSC_VER) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_NETINET_IP_H */
