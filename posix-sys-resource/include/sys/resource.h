#ifndef POSIX_SYS_RESOURCE_SYS_RESOURCE_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define POSIX_SYS_RESOURCE_SYS_RESOURCE_H

/**
 * @file resource.h
 * @brief POSIX sys/resource.h redirection header.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <sys/resource.h>
#else
#include <sys/resource.h>
#endif
#else
#include "../posix-sys-resource.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_RESOURCE_SYS_RESOURCE_H */
