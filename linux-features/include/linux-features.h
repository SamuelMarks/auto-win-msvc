#ifndef LINUX_FEATURES_H
#define LINUX_FEATURES_H

/**
 * @file linux-features.h
 * @brief Polyfill for Linux <features.h>.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by linux-features functions.
 */
enum linux_features_error_code {
  /** @brief Successful operation. */
  LINUX_FEATURES_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  LINUX_FEATURES_ERROR_NULL_POINTER = 1
};

/**
 * @brief Initializes and validates the linux-features module.
 * @param[out] out_status Pointer to an integer that receives the initialized
 * status.
 * @return LINUX_FEATURES_SUCCESS on success, or an error code on failure.
 */
enum linux_features_error_code linux_features_init(int *out_status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_FEATURES_H */
