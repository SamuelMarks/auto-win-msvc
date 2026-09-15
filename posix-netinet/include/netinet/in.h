#ifndef POSIX_NETINET_IN_WRAPPER_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define POSIX_NETINET_IN_WRAPPER_H

/**
 * @file in.h
 * @brief POSIX netinet/in.h redirection header.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <netinet/in.h>
#else
#include <netinet/in.h>
#endif
#else
#include "../posix-netinet-in.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_NETINET_IN_WRAPPER_H */
