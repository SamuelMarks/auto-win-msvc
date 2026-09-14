/* stdint.h - Strict C89 Header */
#ifndef STDINT_WRAPPER_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define STDINT_WRAPPER_H

/**
 * @file stdint.h
 * @brief Standard integer types polyfill for older compilers.
 */

/* clang-format off */
#include "posix-stdint.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STDINT_WRAPPER_H */
