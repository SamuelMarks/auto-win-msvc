#ifndef SYS_STATFS_H
#define SYS_STATFS_H

/**
 * @file statfs.h
 * @brief Forwarding header to linux-sys-statfs.h.
 */

/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32)
#include "../linux-sys-statfs.h"
#else
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/statfs.h>
#else
#include <sys/statfs.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_STATFS_H */
