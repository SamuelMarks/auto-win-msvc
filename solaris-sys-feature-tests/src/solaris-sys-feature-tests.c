/* clang-format off */
#include "solaris-sys-feature-tests.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Initializes and validates the feature tests polyfill.
 * @param[out] out_status Pointer to store the initialization status.
 * @return SOLARIS_SYS_FEATURE_TESTS_SUCCESS or
 * SOLARIS_SYS_FEATURE_TESTS_ERROR_NULL_POINTER.
 */
enum solaris_sys_feature_tests_error_code
solaris_sys_feature_tests_init(int *out_status) {
  if (out_status == NULL) {
    return SOLARIS_SYS_FEATURE_TESTS_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return SOLARIS_SYS_FEATURE_TESTS_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_solaris_sys_feature_tests;
