#ifndef POSIX_NETINET_TCP_H
#define POSIX_NETINET_TCP_H

/**
 * @file posix-netinet-tcp.h
 * @brief POSIX netinet/tcp.h compatibility layer for MSVC.
 */

/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(__MSDOS__) || defined(__WATCOMC__)
#include <stddef.h>
#else
#include <netinet/tcp.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TCP_NODELAY
/** @brief Turn off Nagle's algorithm. */
#define TCP_NODELAY 1
#endif

#ifndef TCP_MAXSEG
/** @brief Limit maximum segment size. */
#define TCP_MAXSEG 2
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_NETINET_TCP_H */
