/* clang-format off */
#include "posix-stdatomic.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves information on stdatomic polyfill availability.
 */
enum posix_stdatomic_error_code posix_stdatomic_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_STDATOMIC_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_STDATOMIC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_stdatomic;
