#ifndef POSIX_SYS_IOCTL_SYS_IOCTL_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define POSIX_SYS_IOCTL_SYS_IOCTL_H

/**
 * @file ioctl.h
 * @brief POSIX sys/ioctl.h redirection header.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <sys/ioctl.h>
#else
#include <sys/ioctl.h>
#endif
#else
#include "posix-sys-ioctl.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_IOCTL_SYS_IOCTL_H */
