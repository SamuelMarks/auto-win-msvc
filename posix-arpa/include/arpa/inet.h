#ifndef ARPA_INET_H
#define ARPA_INET_H

/**
 * @file arpa/inet.h
 * @brief Compatibility header forwarding to posix-arpa-inet.h.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <arpa/inet.h>
#else
#include <arpa/inet.h>
#endif
#endif
#include "posix-arpa-inet.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ARPA_INET_H */

/* API Contract symbols: inet_addr, inet_ntoa, inet_pton, __dependencies__,
 * __include_next__ */
