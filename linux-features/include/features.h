#ifndef FEATURES_H_
#define FEATURES_H_

/**
 * @file features.h
 * @brief Cross-platform <features.h> compatibility shim.
 */

/* clang-format off */
#if defined(__linux__) || defined(__GLIBC__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <features.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FEATURES_H_ */
