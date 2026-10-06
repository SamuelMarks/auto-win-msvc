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

struct ifaddrs {
    struct ifaddrs  *ifa_next;
    char            *ifa_name;
    unsigned int     ifa_flags;
    struct sockaddr *ifa_addr;
    struct sockaddr *ifa_netmask;
    union {
        struct sockaddr *ifu_broadaddr;
        struct sockaddr *ifu_dstaddr;
    } ifa_ifu;
    void            *ifa_data;
};

#ifndef ifa_broadaddr
#define ifa_broadaddr ifa_ifu.ifu_broadaddr
#endif
#ifndef ifa_dstaddr
#define ifa_dstaddr   ifa_ifu.ifu_dstaddr
#endif

#define getifaddrs posix_getifaddrs
#define freeifaddrs posix_freeifaddrs

#if defined(POSIX_SOCKETS_EXPORTS)
__declspec(dllexport)
#else
__declspec(dllimport)
#endif
int posix_getifaddrs(struct ifaddrs **ifap);
#if defined(POSIX_SOCKETS_EXPORTS)
__declspec(dllexport)
#else
__declspec(dllimport)
#endif
void posix_freeifaddrs(struct ifaddrs *ifa);

#ifdef __cplusplus
}
#endif

#endif /* _WIN32 */
/* clang-format on */

#endif /* IFADDRS_H */
