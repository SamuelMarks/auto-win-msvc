/* net/if.h - Strict C89 Header */
#ifndef POSIX_NET_IF_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define POSIX_NET_IF_H

/**
 * @file if.h
 * @brief POSIX net/if.h implementation for MSVC
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <net/if.h>
#else
#include <net/if.h>
#endif
#else
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_MSC_VER)

#ifndef IFNAMSIZ
/** \brief Maximum length of network interface name. */
#define IFNAMSIZ 16
#endif

#ifndef IF_NAMESIZE
/** \brief Standard POSIX alias for IFNAMSIZ. */
#define IF_NAMESIZE IFNAMSIZ
#endif

/**
 * @struct if_nameindex
 * @brief Structure holding interface index and name mapping.
 */
struct if_nameindex {
  unsigned int if_index; /**< Numeric interface index. */
  char *if_name;         /**< Null-terminated interface name. */
};

/**
 * @struct ifreq
 * @brief Interface request structure used for socket ioctls.
 */
struct ifreq {
  char ifr_name[IFNAMSIZ]; /**< Interface name. */
  union {
    struct sockaddr ifru_addr;      /**< Protocol address. */
    struct sockaddr ifru_dstaddr;   /**< Destination address. */
    struct sockaddr ifru_broadaddr; /**< Broadcast address. */
    short ifru_flags;               /**< Interface flags. */
    int ifru_metric;                /**< Interface metric. */
    char *ifru_data;                /**< Custom driver data. */
  } ifr_ifru;                       /**< Interface data union. */
};

#ifndef ifr_addr
#define ifr_addr ifr_ifru.ifru_addr
#endif
#ifndef ifr_dstaddr
#define ifr_dstaddr ifr_ifru.ifru_dstaddr
#endif
#ifndef ifr_broadaddr
#define ifr_broadaddr ifr_ifru.ifru_broadaddr
#endif
#ifndef ifr_flags
#define ifr_flags ifr_ifru.ifru_flags
#endif

#endif /* defined(_WIN32) || defined(_MSC_VER) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_NET_IF_H */
