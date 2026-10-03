#ifndef SOLARIS_SYS_FEATURE_TESTS_H
#define SOLARIS_SYS_FEATURE_TESTS_H

/**
 * @file solaris-sys-feature-tests.h
 * @brief Polyfill for Solaris <sys/feature_tests.h>.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes and validates the solaris-sys-feature-tests module.
 * @param[out] out_status Pointer to an integer that receives the initialized
 * status.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on
 * failure.
 */
auto_win_msvc_error_t solaris_sys_feature_tests_init(int *out_status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SOLARIS_SYS_FEATURE_TESTS_H */
