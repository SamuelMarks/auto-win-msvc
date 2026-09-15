#ifndef POSIX_NETINET_TCP_WRAPPER_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define POSIX_NETINET_TCP_WRAPPER_H

/**
 * @file tcp.h
 * @brief POSIX netinet/tcp.h redirection header.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <netinet/tcp.h>
#else
#include <netinet/tcp.h>
#endif
#else
#include "../posix-netinet-tcp.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_NETINET_TCP_WRAPPER_H */
