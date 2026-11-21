#ifndef STDINT_WRAPPER_H

/**
 * @file stdint.h
 * @brief Standard integer types polyfill for older compilers.
 */

/* clang-format off */
#if defined(__EMSCRIPTEN__)
#pragma GCC system_header
#include_next <stdint.h>
#else
#include "posix-stdint.h"
#endif
/* clang-format on */

#ifndef STDINT_WRAPPER_H
#define STDINT_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STDINT_WRAPPER_H */
#endif /* STDINT_WRAPPER_H */
