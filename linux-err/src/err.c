#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__) ||          \
    defined(__linux__) || defined(__CYGWIN__)
/**
 * @file err.c
 * @brief Implementation of BSD/Linux <err.h> functions.
 */

/* clang-format off */
#include "err.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#if (!defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&  \
     !defined(__NetBSD__)) ||                                                  \
    defined(_MSC_VER) || defined(_WIN32) || defined(__WATCOMC__) ||            \
    defined(__DOS__) || defined(__MSDOS__)

static FILE *g_err_file = NULL;
static void (*g_err_exit)(int) = NULL;

static FILE *get_err_stream(void) {
  return (g_err_file != NULL) ? g_err_file : stderr;
}
#endif

#if (!defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&  \
     !defined(__NetBSD__)) ||                                                  \
    defined(_MSC_VER) || defined(_WIN32) || defined(__WATCOMC__) ||            \
    defined(__DOS__) || defined(__MSDOS__)

void err_set_file(void *vfp) { g_err_file = (FILE *)vfp; }

void err_set_exit(void (*ef)(int)) { g_err_exit = ef; }

void vwarnc(int code, const char *fmt, va_list args) {
  FILE *fp = get_err_stream();
#if defined(_MSC_VER)
  char err_buf[256];
  strerror_s(err_buf, sizeof(err_buf), code);
#else
  const char *err_buf = strerror(code);
#endif
  if (fmt != NULL) {
    vfprintf(fp, fmt, args);
    fprintf(fp, ": %s\n", err_buf);
  } else {
    fprintf(fp, "%s\n", err_buf);
  }
}

void verrc(int eval, int code, const char *fmt, va_list args) {
  vwarnc(code, fmt, args);
  if (g_err_exit != NULL) {
    g_err_exit(eval);
  }
  exit(eval);
}

void errc(int eval, int code, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  verrc(eval, code, fmt, args);
}

void warnc(int code, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vwarnc(code, fmt, args);
  va_end(args);
}

#endif /* !BSD && !__APPLE__ */

#if defined(_MSC_VER) || defined(_WIN32) || defined(__WATCOMC__) ||            \
    defined(__DOS__) || defined(__MSDOS__)

void vwarn(const char *fmt, va_list args) { vwarnc(errno, fmt, args); }

void vwarnx(const char *fmt, va_list args) {
  FILE *fp = get_err_stream();
  if (fmt != NULL) {
    vfprintf(fp, fmt, args);
  }
  fprintf(fp, "\n");
}

void verr(int eval, const char *fmt, va_list args) {
  vwarn(fmt, args);
  if (g_err_exit != NULL) {
    g_err_exit(eval);
  }
  exit(eval);
}

void verrx(int eval, const char *fmt, va_list args) {
  vwarnx(fmt, args);
  if (g_err_exit != NULL) {
    g_err_exit(eval);
  }
  exit(eval);
}

void err(int eval, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  verr(eval, fmt, args);
}

void errx(int eval, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  verrx(eval, fmt, args);
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

#endif /* _MSC_VER || _WIN32 || __WATCOMC__ || __DOS__ || __MSDOS__ */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_err_c;

#endif
typedef int make_iso_compilers_happy_mega_err_c;
