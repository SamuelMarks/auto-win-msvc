#ifndef SOLARIS_SYS_FEATURE_TESTS_H
#define SOLARIS_SYS_FEATURE_TESTS_H

/**
 * @file solaris-sys-feature-tests.h
 * @brief Polyfill for Solaris <sys/feature_tests.h>.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by solaris-sys-feature-tests functions.
 */
enum solaris_sys_feature_tests_error_code {
  /** @brief Successful operation. */
  SOLARIS_SYS_FEATURE_TESTS_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  SOLARIS_SYS_FEATURE_TESTS_ERROR_NULL_POINTER = 1
};

/**
 * @brief Initializes and validates the solaris-sys-feature-tests module.
 * @param[out] out_status Pointer to an integer that receives the initialized
 * status.
 * @return SOLARIS_SYS_FEATURE_TESTS_SUCCESS on success, or an error code on
 * failure.
 */
enum solaris_sys_feature_tests_error_code
solaris_sys_feature_tests_init(int *out_status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SOLARIS_SYS_FEATURE_TESTS_H */
