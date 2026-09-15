/**
 * @file posix-types.c
 * @brief Implementation of posix-types polyfill.
 */

/* clang-format off */
#include "posix-types.h"
#include <stddef.h>
/* clang-format on */

/**
 * @brief Retrieves information on posix-types availability.
 */
enum posix_types_error_code posix_types_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_TYPES_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_TYPES_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_types;
