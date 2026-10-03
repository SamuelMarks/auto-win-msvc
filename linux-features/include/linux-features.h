#ifndef LINUX_FEATURES_H
#define LINUX_FEATURES_H

/**
 * @file linux-features.h
 * @brief Polyfill for Linux <features.h>.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes and validates the linux-features module.
 * @param[out] out_status Pointer to an integer that receives the initialized
 * status.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t linux_features_init(int *out_status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_FEATURES_H */
