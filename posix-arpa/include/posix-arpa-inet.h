/* posix-arpa-inet.h - Strict C89 Header */
#ifndef POSIX_ARPA_INET_H
#define POSIX_ARPA_INET_H

/**
 * @file posix-arpa-inet.h
 * @brief POSIX arpa/inet.h implementation for MSVC
 *
 * This header provides the POSIX inet_aton function
 * implemented using safe Microsoft CRT extensions.
 */

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Converts the Internet host address cp from the IPv4 numbers-and-dots notation into binary form.
 *
 * @param cp The input IP address string.
 * @param inp Pointer to a struct in_addr where the result will be stored.
 * @return 1 if the address is valid, 0 if not.
 */
int posix_inet_aton(const char *cp, struct in_addr *inp);

#ifdef __cplusplus
}
#endif

#else /* Not MSVC/Windows */

#include <arpa/inet.h>

#endif /* defined(_MSC_VER) || defined(_WIN32) */

#endif /* POSIX_ARPA_INET_H */
