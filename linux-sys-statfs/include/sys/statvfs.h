#ifndef SYS_STATVFS_H
#define SYS_STATVFS_H

/**
 * @file statvfs.h
 * @brief Forwarding header to linux-sys-statfs.h for POSIX statvfs support.
 */

/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32)
#include "../linux-sys-statfs.h"
#else
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/statvfs.h>
#else
#include <sys/statvfs.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_STATVFS_H */
