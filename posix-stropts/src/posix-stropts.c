/* clang-format off */
#include "posix-stropts.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves information on stropts polyfill availability.
 */
enum posix_stropts_error_code posix_stropts_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_STROPTS_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_STROPTS_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_stropts;
