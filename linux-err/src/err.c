/* clang-format off */
#include "err.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)

void vwarn(const char *fmt, va_list args) {
  int saved_errno;
#if defined(_MSC_VER)
  char err_buf[256];
#else
  const char *err_buf;
#endif

  saved_errno = errno;
#if defined(_MSC_VER)
  strerror_s(err_buf, sizeof(err_buf), saved_errno);
#else
  err_buf = strerror(saved_errno);
#endif

  if (fmt != NULL) {
    vfprintf(stderr, fmt, args);
    fprintf(stderr, ": %s\n", err_buf);
  } else {
    fprintf(stderr, "%s\n", err_buf);
  }
}

void vwarnx(const char *fmt, va_list args) {
  if (fmt != NULL) {
    vfprintf(stderr, fmt, args);
  }
  fprintf(stderr, "\n");
}

void verr(int eval, const char *fmt, va_list args) {
  vwarn(fmt, args);
  exit(eval);
}

void verrx(int eval, const char *fmt, va_list args) {
  vwarnx(fmt, args);
  exit(eval);
}

void err(int eval, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  verr(eval, fmt, args);
  va_end(args);
}

void errx(int eval, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  verrx(eval, fmt, args);
  va_end(args);
}

void warn(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vwarn(fmt, args);
  va_end(args);
}

void warnx(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vwarnx(fmt, args);
  va_end(args);
}

#endif /* _MSC_VER || _WIN32 */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_err_c;
