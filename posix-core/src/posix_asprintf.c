/**
 * @file posix_asprintf.c
 * @brief Portable C89 implementation of vasprintf and asprintf.
 */

/* clang-format off */
#include "posix-core.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
/* clang-format on */

#ifndef va_copy
#ifdef __va_copy
#define va_copy(dest, src) __va_copy((dest), (src))
#else
#define va_copy(dest, src) ((dest) = (src))
#endif
#endif

#if (!defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)) &&               \
    !defined(__DOS__) && !defined(__MSDOS__) && !defined(__WATCOMC__)

#ifndef POSIX_ASPRINTF_MOCK_vsclen
static int default_asprintf_vsclen(const char *fmt, va_list ap)
    POSIX_CORE_PRINTF_FORMAT(1, 0);
static int default_asprintf_vsclen(const char *fmt, va_list ap) {
#if defined(_MSC_VER)
  return _vscprintf(fmt, ap);
#else
  return vsnprintf(NULL, 0, fmt, ap);
#endif
}
#define POSIX_ASPRINTF_MOCK_vsclen default_asprintf_vsclen
#endif

#ifndef POSIX_ASPRINTF_MOCK_malloc
static void *default_asprintf_malloc(size_t sz) { return malloc(sz); }
#define POSIX_ASPRINTF_MOCK_malloc default_asprintf_malloc
#endif

static int posix_vasprintf_impl(char **strp, const char *fmt, va_list ap)
    POSIX_CORE_PRINTF_FORMAT(2, 0);

static int posix_vasprintf_impl(char **strp, const char *fmt, va_list ap) {
  int len;
  char *buf;
  va_list ap_copy;
  if (strp == NULL || fmt == NULL) {
    return -1;
  }
  va_copy(ap_copy, ap);
  len = POSIX_ASPRINTF_MOCK_vsclen(fmt, ap_copy);
  va_end(ap_copy);
  if (len < 0) {
    *strp = NULL;
    return -1;
  }
  buf = (char *)POSIX_ASPRINTF_MOCK_malloc((size_t)len + 1);
  if (buf == NULL) {
    *strp = NULL;
    return -1;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  (void)vsnprintf_s(buf, (size_t)len + 1, _TRUNCATE, fmt, ap);
#elif defined(_MSC_VER)
  (void)_vsnprintf(buf, (size_t)len + 1, fmt, ap);
  buf[len] = '\0';
#else
  (void)vsnprintf(buf, (size_t)len + 1, fmt, ap);
#endif
  *strp = buf;
  return len;
}

/**
 * @brief Allocates a string large enough to hold the formatted output using a
 * va_list.
 * @param[out] strp Pointer receiving newly allocated string (or NULL on error).
 * @param[in] fmt Format string.
 * @param[in] ap Variable argument list.
 * @return Number of bytes printed (excluding NUL), or -1 on error.
 */
int vasprintf(char **strp, const char *fmt, va_list ap) {
  return posix_vasprintf_impl(strp, fmt, ap);
}

/**
 * @brief Allocates a string large enough to hold the formatted output.
 * @param[out] strp Pointer receiving newly allocated string (or NULL on error).
 * @param[in] fmt Format string.
 * @return Number of bytes printed (excluding NUL), or -1 on error.
 */
int asprintf(char **strp, const char *fmt, ...) {
  va_list ap;
  int ret;
  va_start(ap, fmt);
  ret = vasprintf(strp, fmt, ap);
  va_end(ap);
  return ret;
}

#endif
typedef int make_iso_compilers_happy_posix_asprintf;
