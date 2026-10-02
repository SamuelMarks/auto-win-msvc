/* clang-format off */
#include "err.h"
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
#include <errno.h>

void err(int eval, const char *fmt, ...) {
  (void)eval;
  (void)fmt;
  errno = ENOSYS;
}

void errx(int eval, const char *fmt, ...) {
  (void)eval;
  (void)fmt;
  errno = ENOSYS;
}

void verr(int eval, const char *fmt, va_list args) {
  (void)eval;
  (void)fmt;
  (void)args;
  errno = ENOSYS;
}

void verrx(int eval, const char *fmt, va_list args) {
  (void)eval;
  (void)fmt;
  (void)args;
  errno = ENOSYS;
}

void vwarn(const char *fmt, va_list args) {
  (void)fmt;
  (void)args;
  errno = ENOSYS;
}

void vwarnx(const char *fmt, va_list args) {
  (void)fmt;
  (void)args;
  errno = ENOSYS;
}

void warn(const char *fmt, ...) {
  (void)fmt;
  errno = ENOSYS;
}

void warnx(const char *fmt, ...) {
  (void)fmt;
  errno = ENOSYS;
}

#endif /* _MSC_VER || _WIN32 */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_err_c;
