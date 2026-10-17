/* netinet/ip.h - Strict C89 Header */
#ifndef POSIX_NETINET_IP_H
#if defined(__GNUC__)
#endif
#define POSIX_NETINET_IP_H

/**
 * @file ip.h
 * @brief POSIX netinet/ip.h implementation for MSVC and non-Linux targets.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <netinet/ip.h>
#else
#include <netinet/ip.h>
#endif
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
#else
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
#include "../posix-netinet-in.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_MSC_VER) || defined(__APPLE__)
#ifndef _STRUCT_IPHDR_DEFINED
#define _STRUCT_IPHDR_DEFINED 1
/**
 * @brief Linux-compatible IPv4 header structure.
 */
struct iphdr {
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  unsigned int version : 4; /**< IP version */
  unsigned int ihl : 4;     /**< Internet header length */
#else
  unsigned int ihl : 4;     /**< Internet header length */
  unsigned int version : 4; /**< IP version */
#endif
  uint8_t tos;       /**< Type of service */
  uint16_t tot_len;  /**< Total length */
  uint16_t id;       /**< Identification */
  uint16_t frag_off; /**< Fragment offset field */
  uint8_t ttl;       /**< Time to live */
  uint8_t protocol;  /**< Protocol */
  uint16_t check;    /**< Checksum */
  uint32_t saddr;    /**< Source IPv4 address */
  uint32_t daddr;    /**< Destination IPv4 address */
};
#endif /* _STRUCT_IPHDR_DEFINED */
#endif /* _WIN32 || _MSC_VER || __APPLE__ */

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
