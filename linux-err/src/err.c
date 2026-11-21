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

#ifndef ERR_MOCK_VFPRINTF
#define ERR_MOCK_VFPRINTF vfprintf
#endif

#ifndef ERR_MOCK_FPRINTF
#define ERR_MOCK_FPRINTF fprintf
#endif

#ifndef ERR_DEFAULT_EXIT
#define ERR_DEFAULT_EXIT(status) exit(status)
#endif

static void default_err_terminate(int eval) { ERR_DEFAULT_EXIT(eval); }

static linux_err_terminate_fn_t g_err_terminate = default_err_terminate;

/**
 * @brief Overrides the low-level termination handler used by err*() functions.
 * @param[in] fn Replacement termination callback.
 */
void linux_err_set_terminate_for_test(linux_err_terminate_fn_t fn) {
  g_err_terminate = fn;
}

/**
 * @brief Restores the default termination handler used by err*() functions.
 */
void linux_err_reset_terminate_for_test(void) {
  g_err_terminate = default_err_terminate;
}

#if (!defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&  \
     !defined(__NetBSD__)) ||                                                  \
    defined(_MSC_VER) || defined(_WIN32) || defined(__WATCOMC__) ||            \
    defined(__DOS__) || defined(__MSDOS__)

static FILE *g_err_file = NULL;
static void (*g_err_exit)(int) = NULL;

static FILE *get_err_stream(void) {
  return (g_err_file != NULL) ? g_err_file : stderr;
}

/**
 * @brief Override output FILE* stream used by err*() and warn*().
 */
void err_set_file(void *vfp) { g_err_file = (FILE *)vfp; }

/**
 * @brief Set exit callback invoked before process termination in err*().
 */
void err_set_exit(void (*ef)(int)) { g_err_exit = ef; }

static void vwarnc_impl(int code, const char *fmt, va_list args)
    LINUX_ERR_PRINTF(2, 0);
static void verrc_impl(int eval, int code, const char *fmt, va_list args)
    LINUX_ERR_PRINTF(3, 0);

static void vwarnc_impl(int code, const char *fmt, va_list args) {
  FILE *fp = get_err_stream();
#if defined(_MSC_VER)
  char err_buf[256];
  strerror_s(err_buf, sizeof(err_buf), code);
#else
  const char *err_buf = strerror(code);
#endif
  if (fmt != NULL) {
    ERR_MOCK_VFPRINTF(fp, fmt, args);
    ERR_MOCK_FPRINTF(fp, ": %s\n", err_buf);
  } else {
    ERR_MOCK_FPRINTF(fp, "%s\n", err_buf);
  }
}

static void verrc_impl(int eval, int code, const char *fmt, va_list args) {
  vwarnc_impl(code, fmt, args);
  if (g_err_exit != NULL) {
    g_err_exit(eval);
  }
  g_err_terminate(eval);
}

/**
 * @brief Format warning message using explicit errno value and va_list.
 * @param[in] code Explicit errno code.
 * @param[in] fmt Format string (may be NULL).
 * @param[in] args Variable argument list.
 */
void vwarnc(int code, const char *fmt, va_list args) {
  vwarnc_impl(code, fmt, args);
}

/**
 * @brief Format error message using explicit errno value and va_list, then
 * exit.
 * @param[in] eval Exit status code.
 * @param[in] code Explicit errno code.
 * @param[in] fmt Format string (may be NULL).
 * @param[in] args Variable argument list.
 */
void verrc(int eval, int code, const char *fmt, va_list args) {
  verrc_impl(eval, code, fmt, args);
}

/**
 * @brief Format error message using explicit errno value, then exit.
 * @param[in] eval Exit status code.
 * @param[in] code Explicit errno code.
 * @param[in] fmt Format string (may be NULL).
 */
void errc(int eval, int code, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  verrc(eval, code, fmt, args);
  va_end(args);
}

/**
 * @brief Format warning message using explicit errno value.
 * @param[in] code Explicit errno code.
 * @param[in] fmt Format string (may be NULL).
 */
void warnc(int code, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vwarnc(code, fmt, args);
  va_end(args);
}

#endif /* !BSD && !__APPLE__ */

#if defined(_MSC_VER) || defined(_WIN32) || defined(__WATCOMC__) ||            \
    defined(__DOS__) || defined(__MSDOS__)

/**
 * @brief Display formatted warning message and strerror(errno) using va_list.
 * @param[in] fmt Format string (may be NULL).
 * @param[in] args Variable argument list.
 */
void vwarn(const char *fmt, va_list args) { vwarnc(errno, fmt, args); }

static void vwarnx_impl(const char *fmt, va_list args) LINUX_ERR_PRINTF(1, 0);
static void verr_impl(int eval, const char *fmt, va_list args)
    LINUX_ERR_PRINTF(2, 0);
static void verrx_impl(int eval, const char *fmt, va_list args)
    LINUX_ERR_PRINTF(2, 0);

static void vwarnx_impl(const char *fmt, va_list args) {
  FILE *fp = get_err_stream();
  if (fmt != NULL) {
    ERR_MOCK_VFPRINTF(fp, fmt, args);
  }
  ERR_MOCK_FPRINTF(fp, "\n");
}

static void verr_impl(int eval, const char *fmt, va_list args) {
  vwarnc_impl(errno, fmt, args);
  if (g_err_exit != NULL) {
    g_err_exit(eval);
  }
  g_err_terminate(eval);
}

static void verrx_impl(int eval, const char *fmt, va_list args) {
  vwarnx_impl(fmt, args);
  if (g_err_exit != NULL) {
    g_err_exit(eval);
  }
  g_err_terminate(eval);
}

/**
 * @brief Display formatted warning message without errno string using va_list.
 * @param[in] fmt Format string (may be NULL).
 * @param[in] args Variable argument list.
 */
void vwarnx(const char *fmt, va_list args) { vwarnx_impl(fmt, args); }

/**
 * @brief Display formatted error message and strerror(errno) using va_list,
 * then exit.
 * @param[in] eval Process exit status code.
 * @param[in] fmt Format string (may be NULL).
 * @param[in] args Variable argument list.
 */
void verr(int eval, const char *fmt, va_list args) {
  verr_impl(eval, fmt, args);
}

/**
 * @brief Display formatted error message without errno string using va_list,
 * then exit.
 * @param[in] eval Process exit status code.
 * @param[in] fmt Format string (may be NULL).
 * @param[in] args Variable argument list.
 */
void verrx(int eval, const char *fmt, va_list args) {
  verrx_impl(eval, fmt, args);
}

/**
 * @brief Display formatted error message and strerror(errno), then exit.
 * @param[in] eval Process exit status code.
 * @param[in] fmt Format string (may be NULL).
 */
void err(int eval, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  verr(eval, fmt, args);
  va_end(args);
}

/**
 * @brief Display formatted error message without errno string, then exit.
 * @param[in] eval Process exit status code.
 * @param[in] fmt Format string (may be NULL).
 */
void errx(int eval, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  verrx(eval, fmt, args);
  va_end(args);
}

/**
 * @brief Display formatted warning message and strerror(errno).
 * @param[in] fmt Format string (may be NULL).
 */
void warn(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vwarn(fmt, args);
  va_end(args);
}

/**
 * @brief Display formatted warning message without errno string.
 * @param[in] fmt Format string (may be NULL).
 */
void warnx(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vwarnx(fmt, args);
  va_end(args);
}

#endif /* _MSC_VER || _WIN32 || __WATCOMC__ || __DOS__ || __MSDOS__ */

/**
 * @brief Compilation unit anchor symbol for linux-err coverage.
 * @return Always 0.
 */
int linux_err_dummy_for_coverage(void) { return 0; }

#endif
typedef int make_iso_compilers_happy_mega_err_c;
