/* clang-format off */
#include "haiku-os.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Initializes and validates the haiku-os polyfill.
 */
enum haiku_os_error_code haiku_os_init(int *out_status) {
  if (out_status == NULL) {
    return HAIKU_OS_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return HAIKU_OS_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_haiku_os;
