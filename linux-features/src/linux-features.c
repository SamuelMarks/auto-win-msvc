/* clang-format off */
#include "linux-features.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Initializes and validates the features polyfill.
 * @param[out] out_status Pointer to store initialization status.
 * @return LINUX_FEATURES_SUCCESS or LINUX_FEATURES_ERROR_NULL_POINTER.
 */
enum linux_features_error_code linux_features_init(int *out_status) {
  if (out_status == NULL) {
    return LINUX_FEATURES_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return LINUX_FEATURES_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_features;
