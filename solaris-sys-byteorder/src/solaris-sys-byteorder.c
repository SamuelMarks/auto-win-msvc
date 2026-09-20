/* clang-format off */
#include "solaris-sys-byteorder.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves the current host byte order.
 */
enum solaris_sys_byteorder_error_code
solaris_sys_byteorder_get_byte_order(int *out_byte_order) {
  if (out_byte_order == NULL) {
    return SOLARIS_SYS_BYTEORDER_ERROR_NULL_POINTER;
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
  return SOLARIS_SYS_BYTEORDER_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_solaris_sys_byteorder;
