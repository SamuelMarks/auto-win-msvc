#ifndef SYS_SYSCTL_H
#define SYS_SYSCTL_H

/**
 * @file sysctl.h
 * @brief Polyfill forwarding header for BSD <sys/sysctl.h>.
 */

/* clang-format off */
#if (defined(__APPLE__) && (!defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE))) || \
    defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/sysctl.h>
#endif
#endif
#include "../bsd-sys-sysctl.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_SYSCTL_H */
