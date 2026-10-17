#ifndef SYS_SYSINFO_H
#define SYS_SYSINFO_H

/**
 * @file sysinfo.h
 * @brief Forwarding header to linux-sys-procfs.h for <sys/sysinfo.h>.
 */

/* clang-format off */
#if defined(__linux__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/sysinfo.h>
#else
#include <sys/sysinfo.h>
#endif
#else
#include "../linux-sys-procfs.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_SYSINFO_H */
