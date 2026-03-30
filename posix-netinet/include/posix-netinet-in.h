/* posix-netinet-in.h - Strict C89 Header */
#ifndef POSIX_NETINET_IN_H
#define POSIX_NETINET_IN_H

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#else
#include <netinet/in.h>
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif
#endif /* defined(_MSC_VER) || defined(_WIN32) */

#endif /* POSIX_NETINET_IN_H */




