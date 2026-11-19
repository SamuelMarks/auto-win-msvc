#ifndef MACH_MACH_INIT_H
#define MACH_MACH_INIT_H

/**
 * @file mach/mach_init.h
 * @brief Polyfill for macOS <mach/mach_init.h>.
 */

/* clang-format off */
#if defined(__APPLE__) && (defined(__GNUC__) || defined(__clang__))
#pragma GCC system_header
#include_next <mach/mach_init.h>
#else
#include "mach.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MACH_MACH_INIT_H */
