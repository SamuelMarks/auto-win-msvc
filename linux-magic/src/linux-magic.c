/* clang-format off */
#include "linux/magic.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves the TMPFS magic number.
 */
enum linux_magic_error_code
linux_magic_get_tmpfs_magic(unsigned long *out_magic) {
  if (out_magic == NULL) {
    return LINUX_MAGIC_ERROR_NULL_POINTER;
  }
  *out_magic = TMPFS_MAGIC;
  return LINUX_MAGIC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_magic;
