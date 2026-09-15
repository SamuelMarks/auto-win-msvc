/* clang-format off */
#include "linux-sys-bitops.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Initializes and validates the bitops module.
 * @param[out] out_status Pointer to store initialization status.
 * @return LINUX_SYS_BITOPS_SUCCESS or LINUX_SYS_BITOPS_ERROR_NULL_POINTER.
 */
enum linux_sys_bitops_error_code linux_sys_bitops_init(int *out_status) {
  if (out_status == NULL) {
    return LINUX_SYS_BITOPS_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return LINUX_SYS_BITOPS_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_sys_bitops;
