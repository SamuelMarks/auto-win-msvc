#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/* clang-format off */
#include "bsd-sys-param.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves the maximum path length.
 */
auto_win_msvc_error_t bsd_sys_param_get_maxpathlen(size_t *out_maxlen) {
  if (out_maxlen == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_maxlen = (size_t)MAXPATHLEN;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_bsd_sys_param;

#endif
typedef int make_iso_compilers_happy_mega_batch2_9;
