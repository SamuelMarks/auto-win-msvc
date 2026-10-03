/* clang-format off */
#include "linux-endian.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves the current host byte order.
 */
auto_win_msvc_error_t linux_endian_get_byte_order(int *out_byte_order) {
  if (out_byte_order == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_byte_order = BYTE_ORDER;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_endian;
