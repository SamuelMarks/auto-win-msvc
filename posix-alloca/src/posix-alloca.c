/* clang-format off */
#include "alloca.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves information on alloca availability.
 */
auto_win_msvc_error_t posix_alloca_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_alloca;
