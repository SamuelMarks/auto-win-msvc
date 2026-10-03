/* clang-format off */
#include "solaris-sys-byteorder.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves the current host byte order.
 */
auto_win_msvc_error_t
solaris_sys_byteorder_get_byte_order(int *out_byte_order) {
  if (out_byte_order == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
#if defined(BYTE_ORDER)
  *out_byte_order = BYTE_ORDER;
#elif defined(_BYTE_ORDER)
  *out_byte_order = _BYTE_ORDER;
#elif defined(__BYTE_ORDER)
  *out_byte_order = __BYTE_ORDER;
#else
  *out_byte_order = 1234;
#endif
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_solaris_sys_byteorder;
