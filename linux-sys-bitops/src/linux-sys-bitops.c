/* clang-format off */
#include "linux-sys-bitops.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Initializes and validates the bitops module.
 */
auto_win_msvc_error_t linux_sys_bitops_init(int *out_status) {
  if (out_status == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_status = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_sys_bitops;
