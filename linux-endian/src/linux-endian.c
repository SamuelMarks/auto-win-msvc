/* clang-format off */
#include "linux-endian.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves the current host byte order.
 */
enum linux_endian_error_code linux_endian_get_byte_order(int *out_byte_order) {
  if (out_byte_order == NULL) {
    return LINUX_ENDIAN_ERROR_NULL_POINTER;
  }
  *out_byte_order = BYTE_ORDER;
  return LINUX_ENDIAN_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_endian;
