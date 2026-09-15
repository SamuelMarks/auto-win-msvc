/* clang-format off */
#include "posix-stdint.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves information on stdint polyfill availability.
 */
enum posix_stdint_error_code posix_stdint_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_STDINT_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_STDINT_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_stdint;
