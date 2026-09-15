/* clang-format off */
#include "linux-hv-hloop.h"
#include <stddef.h>
/* clang-format on */

/** @brief Initializes and validates the linux-hv-hloop module. */
enum linux_hv_hloop_error_code linux_hv_hloop_init(int *out_status) {
  if (out_status == NULL) {
    return LINUX_HV_HLOOP_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return LINUX_HV_HLOOP_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_hv_hloop;
