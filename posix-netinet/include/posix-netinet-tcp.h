/* posix-netinet-tcp.h - Strict C89 Header */
#ifndef POSIX_NETINET_TCP_H
#define POSIX_NETINET_TCP_H

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#else

#include <netinet/tcp.h>

#endif /* defined(_MSC_VER) || defined(_WIN32) */

#endif /* POSIX_NETINET_TCP_H */
