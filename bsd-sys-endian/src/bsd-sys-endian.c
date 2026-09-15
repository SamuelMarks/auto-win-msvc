/* clang-format off */
#include "bsd-sys-endian.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves the current host byte order.
 * @param[out] out_byte_order Pointer to store the byte order value.
 * @return BSD_SYS_ENDIAN_SUCCESS or BSD_SYS_ENDIAN_ERROR_NULL_POINTER.
 */
enum bsd_sys_endian_error_code
bsd_sys_endian_get_byte_order(int *out_byte_order) {
  if (out_byte_order == NULL) {
    return BSD_SYS_ENDIAN_ERROR_NULL_POINTER;
  }
  *out_byte_order = _BYTE_ORDER;
  return BSD_SYS_ENDIAN_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_bsd_sys_endian;
