#ifndef POSIX_CORE_UNISTD_H
#define POSIX_CORE_UNISTD_H

#if defined(__GNUC__)
#pragma GCC system_header
#endif

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <unistd.h>
#else
#if !defined(_MSC_VER)
#include <unistd.h>
#endif
#endif
#else
#include "posix-core.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file unistd.h
 * @brief Standard symbolic constants and types header.
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_CORE_UNISTD_H */
