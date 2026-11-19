#ifndef POSIX_NETDB_RESOLV_H
#define POSIX_NETDB_RESOLV_H

/**
 * @file resolv.h
 * @brief Cross-platform <resolv.h> compatibility shim.
 */

/* clang-format off */
#if !defined(_WIN32) && !defined(__WATCOMC__) && !defined(__MSDOS__) &&        \
    !defined(__DOS__)
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <resolv.h>
#else
#include <resolv.h>
#endif
#else
#include "posix-resolv.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_NETDB_RESOLV_H */
