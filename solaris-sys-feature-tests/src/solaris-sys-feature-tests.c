/* clang-format off */
#include "solaris-sys-feature-tests.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Initializes and validates the feature tests polyfill.
 */
auto_win_msvc_error_t solaris_sys_feature_tests_init(int *out_status) {
  if (out_status == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_status = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_solaris_sys_feature_tests;
