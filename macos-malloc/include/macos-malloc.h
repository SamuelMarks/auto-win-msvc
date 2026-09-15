#ifndef MACOS_MALLOC_H
#define MACOS_MALLOC_H

/**
 * @file macos-malloc.h
 * @brief Polyfill for macOS <malloc/malloc.h>.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by macos-malloc functions.
 */
enum macos_malloc_error_code {
  /** @brief Successful operation. */
  MACOS_MALLOC_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  MACOS_MALLOC_ERROR_NULL_POINTER = 1,
  /** @brief Operation not supported on this platform. */
  MACOS_MALLOC_ERROR_NOT_SUPPORTED = 2
};

/**
 * @brief Initializes and validates the macos-malloc module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return MACOS_MALLOC_SUCCESS on success, or an error code on failure.
 */
enum macos_malloc_error_code macos_malloc_init(int *out_status);

/**
 * @brief Returns the good size for an allocation request.
 * @param size Requested allocation size.
 * @param[out] out_good_size Pointer receiving the recommended size.
 * @return MACOS_MALLOC_SUCCESS on success, or an error code on failure.
 */
enum macos_malloc_error_code macos_malloc_good_size(size_t size,
                                                    size_t *out_good_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MACOS_MALLOC_H */
