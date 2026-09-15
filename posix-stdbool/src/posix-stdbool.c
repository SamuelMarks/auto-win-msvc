/* clang-format off */
#include "posix-stdbool.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves information on stdbool polyfill availability.
 */
enum posix_stdbool_error_code posix_stdbool_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_STDBOOL_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_STDBOOL_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_stdbool;
