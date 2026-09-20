/* clang-format off */
#include "vm/vm_param.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves the HOST_VM_INFO constant value.
 */
enum bsd_vm_param_error_code bsd_vm_param_get_host_vm_info(int *out_info) {
  if (out_info == NULL) {
    return BSD_VM_PARAM_ERROR_NULL_POINTER;
  }
  *out_info = HOST_VM_INFO;
  return BSD_VM_PARAM_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_bsd_vm_param;
