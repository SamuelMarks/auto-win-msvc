#ifndef MACH_TASK_H
#define MACH_TASK_H

/**
 * @file mach/task.h
 * @brief Polyfill for macOS <mach/task.h>.
 */

/* clang-format off */
#if defined(__APPLE__) && (defined(__GNUC__) || defined(__clang__))
#pragma GCC system_header
#include_next <mach/task.h>
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

#endif /* MACH_TASK_H */
