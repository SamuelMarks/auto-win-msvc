/* stdbool.h - Strict C89 Header */
#ifndef STDBOOL_WRAPPER_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define STDBOOL_WRAPPER_H

/**
 * @file stdbool.h
 * @brief Standard boolean definitions polyfill for older compilers.
 */

/* clang-format off */
#include "posix-stdbool.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STDBOOL_WRAPPER_H */
