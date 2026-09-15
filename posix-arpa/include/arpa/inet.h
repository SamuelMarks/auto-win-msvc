#ifndef ARPA_INET_H
#define ARPA_INET_H

/**
 * @file arpa/inet.h
 * @brief Compatibility header forwarding to posix-arpa-inet.h.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
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
