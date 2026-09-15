/* clang-format off */
#include "macos-malloc.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Initializes and validates the macos-malloc module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return MACOS_MALLOC_SUCCESS on success, or an error code on failure.
 */
enum macos_malloc_error_code macos_malloc_init(int *out_status) {
  if (out_status == NULL) {
    return MACOS_MALLOC_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return MACOS_MALLOC_SUCCESS;
}

/**
 * @brief Returns the good size for an allocation request.
 * @param size Requested allocation size.
 * @param[out] out_good_size Pointer receiving the recommended size.
 * @return MACOS_MALLOC_SUCCESS on success, or an error code on failure.
 */
enum macos_malloc_error_code macos_malloc_good_size(size_t size,
                                                    size_t *out_good_size) {
  if (out_good_size == NULL) {
    return MACOS_MALLOC_ERROR_NULL_POINTER;
  }
  /* Round up to nearest 16-byte boundary */
  *out_good_size = (size + 15) & ~((size_t)15);
  return MACOS_MALLOC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_macos_malloc;
