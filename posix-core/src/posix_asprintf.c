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

/** \brief vasprintf function. */
int vasprintf(char **strp, const char *fmt, va_list ap) {
  int len;
  char *buf;
  va_list ap_copy;
  if (strp == NULL || fmt == NULL) {
    return -1;
  }
  va_copy(ap_copy, ap);
#if defined(_MSC_VER)
  len = _vscprintf(fmt, ap_copy);
#else
  len = vsnprintf(NULL, 0, fmt, ap_copy);
#endif
  va_end(ap_copy);
  if (len < 0) {
    *strp = NULL;
    return -1;
  }
  buf = (char *)malloc((size_t)len + 1);
  if (buf == NULL) {
    *strp = NULL;
    return -1;
  }
#if defined(_MSC_VER)
  vsnprintf_s(buf, (size_t)len + 1, (size_t)len, fmt, ap);
#else
  vsnprintf(buf, (size_t)len + 1, fmt, ap);
#endif
  buf[len] = '\0';
  *strp = buf;
  return len;
}

/** \brief asprintf function. */
int asprintf(char **strp, const char *fmt, ...) {
  va_list ap;
  int ret;
  va_start(ap, fmt);
  ret = vasprintf(strp, fmt, ap);
  va_end(ap);
  return ret;
}
