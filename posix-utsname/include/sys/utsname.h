#ifndef SYS_UTSNAME_H
#define SYS_UTSNAME_H

/**
 * @file utsname.h
 * @brief Forwarding header to posix-utsname.h.
 */

/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32)
#include "../posix-utsname.h"
#else
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/utsname.h>
#else
#include <sys/utsname.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_UTSNAME_H */
