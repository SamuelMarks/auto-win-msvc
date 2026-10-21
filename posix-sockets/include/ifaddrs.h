#ifndef IFADDRS_H
#define IFADDRS_H

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <ifaddrs.h>
#else
#include <ifaddrs.h>
#endif
#else

#include <sys/types.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Network interface addresses structure.
 */
struct ifaddrs {
  struct ifaddrs *ifa_next;     /**< Next item in list. */
  char *ifa_name;               /**< Name of interface. */
  unsigned int ifa_flags;       /**< Flags from SIOCGIFFLAGS. */
  struct sockaddr *ifa_addr;    /**< Address of interface. */
  struct sockaddr *ifa_netmask; /**< Netmask of interface. */
  union {
    struct sockaddr *ifu_broadaddr; /**< Broadcast address of interface. */
    struct sockaddr *ifu_dstaddr;   /**< Point-to-point destination address. */
  } ifa_ifu;                        /**< Address union. */
  void *ifa_data;                   /**< Address-specific data. */
};

#ifndef ifa_broadaddr
#define ifa_broadaddr ifa_ifu.ifu_broadaddr
#endif
#ifndef ifa_dstaddr
#define ifa_dstaddr ifa_ifu.ifu_dstaddr
#endif

#define getifaddrs posix_getifaddrs
#define freeifaddrs posix_freeifaddrs

/**
 * @brief Creates a linked list of structures describing the network interfaces
 * of the local system.
 * @param[out] ifap Pointer receiving the head of the interface list.
 * @return 0 on success, or -1 on error.
 */
int posix_getifaddrs(struct ifaddrs **ifap);

/**
 * @brief Frees the memory allocated by getifaddrs().
 * @param[in] ifa Head of the interface list to free.
 */
void posix_freeifaddrs(struct ifaddrs *ifa);

#ifdef __cplusplus
}
#endif

#endif /* _WIN32 */
/* clang-format on */

#endif /* IFADDRS_H */
