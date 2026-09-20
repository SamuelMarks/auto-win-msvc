/* clang-format off */
#include "bsd-sys-param.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves the maximum path length.
 */
enum bsd_sys_param_error_code bsd_sys_param_get_maxpathlen(size_t *out_maxlen) {
  if (out_maxlen == NULL) {
    return BSD_SYS_PARAM_ERROR_NULL_POINTER;
  }
  *out_maxlen = (size_t)MAXPATHLEN;
  return BSD_SYS_PARAM_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_bsd_sys_param;
