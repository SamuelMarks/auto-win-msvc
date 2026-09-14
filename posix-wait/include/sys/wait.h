/* sys/wait.h - Strict C89 Header */
#ifndef SYS_WAIT_WRAPPER_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define SYS_WAIT_WRAPPER_H

/**
 * @file wait.h
 * @brief Standard POSIX sys/wait.h header wrapper.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <sys/wait.h>
#else
#include <sys/wait.h>
#endif
#else
#include "posix-wait.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_WAIT_WRAPPER_H */
