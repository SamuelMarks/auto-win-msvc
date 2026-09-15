/* clang-format off */
#include "bsd-machine-endian.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves the current host byte order.
 * @param[out] out_byte_order Pointer to store the byte order value.
 * @return BSD_MACHINE_ENDIAN_SUCCESS or BSD_MACHINE_ENDIAN_ERROR_NULL_POINTER.
 */
enum bsd_machine_endian_error_code
bsd_machine_endian_get_byte_order(int *out_byte_order) {
  if (out_byte_order == NULL) {
    return BSD_MACHINE_ENDIAN_ERROR_NULL_POINTER;
  }
  *out_byte_order = _BYTE_ORDER;
  return BSD_MACHINE_ENDIAN_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_bsd_machine_endian;
