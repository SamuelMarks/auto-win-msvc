/* clang-format off */
#include "AvailabilityMacros.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves the max allowed macOS version.
 * @param[out] out_version Pointer to store the version value.
 * @return MACOS_AVAILABILITYMACROS_SUCCESS or
 * MACOS_AVAILABILITYMACROS_ERROR_NULL_POINTER.
 */
enum macos_availabilitymacros_error_code
macos_availabilitymacros_get_version(int *out_version) {
  if (out_version == NULL) {
    return MACOS_AVAILABILITYMACROS_ERROR_NULL_POINTER;
  }
  *out_version = MAC_OS_X_VERSION_MAX_ALLOWED;
  return MACOS_AVAILABILITYMACROS_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_macos_availabilitymacros;
