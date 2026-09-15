/* clang-format off */
#include "alloca.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves information on alloca availability.
 */
enum posix_alloca_error_code posix_alloca_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_ALLOCA_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_ALLOCA_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_alloca;
