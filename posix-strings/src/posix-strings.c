/**
 * @file posix-strings.c
 * @brief Implementation of posix-strings polyfills.
 */

/* clang-format off */
#include "posix-strings.h"
#include <stddef.h>

#if defined(_MSC_VER)
#include <intrin.h>
/* clang-format on */

#pragma intrinsic(_BitScanForward)

#if defined(_WIN64)
#pragma intrinsic(_BitScanForward64)
#endif

/** @brief Finds the first (least significant) bit set in an integer. */
int ffs(int i) {
  unsigned long index;
  if (i == 0) {
    return 0;
  }
  if (_BitScanForward(&index, (unsigned long)i)) {
    return (int)index + 1;
  }
  return 0;
}

/** @brief Finds the first (least significant) bit set in a long integer. */
int ffsl(long i) {
  unsigned long index;
  if (i == 0) {
    return 0;
  }
  if (_BitScanForward(&index, (unsigned long)i)) {
    return (int)index + 1;
  }
  return 0;
}

/** @brief Finds the first (least significant) bit set in a long long integer.
 */
int ffsll(posix_strings_llong i) {
  unsigned long index;
  if (i == 0) {
    return 0;
  }
#if defined(_WIN64)
  if (_BitScanForward64(&index, (unsigned __int64)i)) {
    return (int)index + 1;
  }
#else
  if ((unsigned long)i != 0) {
    if (_BitScanForward(&index, (unsigned long)i)) {
      return (int)index + 1;
    }
  } else {
    if (_BitScanForward(&index, (unsigned long)(i >> 32))) {
      return (int)index + 33;
    }
  }
#endif
  return 0;
}

#endif /* _MSC_VER */

/**
 * @brief Retrieves information on posix-strings polyfill availability.
 */
enum posix_strings_error_code posix_strings_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_STRINGS_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_STRINGS_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_strings;
