/* clang-format off */
#include "linux-features.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Initializes and validates the features polyfill.
 */
auto_win_msvc_error_t linux_features_init(int *out_status) {
  if (out_status == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_status = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_features;
