/* clang-format off */
#include "linux-sys-procfs.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Initializes and validates the linux-sys-procfs module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return LINUX_SYS_PROCFS_SUCCESS on success, or an error code on failure.
 */
enum linux_sys_procfs_error_code linux_sys_procfs_init(int *out_status) {
  if (out_status == NULL) {
    return LINUX_SYS_PROCFS_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return LINUX_SYS_PROCFS_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_sys_procfs;
