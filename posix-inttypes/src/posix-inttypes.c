/* clang-format off */
#include "posix-inttypes.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves information on inttypes polyfill availability.
 */
enum posix_inttypes_error_code posix_inttypes_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_INTTYPES_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_INTTYPES_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_inttypes;
