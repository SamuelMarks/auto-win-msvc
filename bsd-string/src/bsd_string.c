/* clang-format off */
#include "bsd/string.h"
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
#include <errno.h>

size_t strlcpy(char *dst, const char *src, size_t size) {
  (void)dst;
  (void)src;
  (void)size;
  errno = ENOSYS;
  return (size_t)-1;
}

size_t strlcat(char *dst, const char *src, size_t size) {
  (void)dst;
  (void)src;
  (void)size;
  errno = ENOSYS;
  return (size_t)-1;
}

#endif /* _MSC_VER || _WIN32 */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_bsd_string_c;
