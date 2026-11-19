/* clang-format off */
#include "greatest.h"
#include "err.h"
#include <errno.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

extern int linux_err_dummy_for_coverage(void);

static int g_mock_exit_status = -1;
static int g_custom_exit_called = 0;
static char g_mock_stderr_buf[2048];

static void mock_exit_return(int status) { g_mock_exit_status = status; }

static void custom_exit_cb(int eval) { g_custom_exit_called = eval; }

static int mock_vfprintf(FILE *stream, const char *format, va_list arg)
    LINUX_ERR_PRINTF(2, 0);
static int mock_fprintf(FILE *stream, const char *format, ...)
    LINUX_ERR_PRINTF(2, 3);

static int mock_vfprintf(FILE *stream, const char *format, va_list arg) {
  size_t cur_len = strlen(g_mock_stderr_buf);
  (void)stream;
  if (cur_len >= sizeof(g_mock_stderr_buf) - 1) {
    return 0;
  }
#if defined(_MSC_VER)
  return vsprintf_s(g_mock_stderr_buf + cur_len,
                    sizeof(g_mock_stderr_buf) - cur_len, format, arg);
#else
  return vsprintf(g_mock_stderr_buf + cur_len, format, arg);
#endif
}

static int mock_fprintf(FILE *stream, const char *format, ...) {
  int ret;
  va_list arg;
  va_start(arg, format);
  ret = mock_vfprintf(stream, format, arg);
  va_end(arg);
  return ret;
}

void my_vwarn(const char *fmt, va_list args) LINUX_ERR_PRINTF(1, 0);
void my_vwarnx(const char *fmt, va_list args) LINUX_ERR_PRINTF(1, 0);
void my_verr(int eval, const char *fmt, va_list args) LINUX_ERR_PRINTF(2, 0);
void my_verrx(int eval, const char *fmt, va_list args) LINUX_ERR_PRINTF(2, 0);
void my_err(int eval, const char *fmt, ...) LINUX_ERR_PRINTF(2, 3);
void my_errx(int eval, const char *fmt, ...) LINUX_ERR_PRINTF(2, 3);
void my_warn(const char *fmt, ...) LINUX_ERR_PRINTF(1, 2);
void my_warnx(const char *fmt, ...) LINUX_ERR_PRINTF(1, 2);
void my_vwarnc(int code, const char *fmt, va_list args) LINUX_ERR_PRINTF(2, 0);
void my_verrc(int eval, int code, const char *fmt, va_list args)
    LINUX_ERR_PRINTF(3, 0);
void my_warnc(int code, const char *fmt, ...) LINUX_ERR_PRINTF(2, 3);
void my_errc(int eval, int code, const char *fmt, ...) LINUX_ERR_PRINTF(3, 4);

#ifndef _WIN32
#define DEFINED_WIN32_FOR_TEST
#define _WIN32 1
#endif

#define ERR_DEFAULT_EXIT(status) mock_exit_return(status)
#define ERR_MOCK_VFPRINTF mock_vfprintf
#define ERR_MOCK_FPRINTF mock_fprintf

#define vwarn my_vwarn
#define vwarnx my_vwarnx
#define verr my_verr
#define verrx my_verrx
#define err my_err
#define errx my_errx
#define warn my_warn
#define warnx my_warnx
#define vwarnc my_vwarnc
#define verrc my_verrc
#define warnc my_warnc
#define errc my_errc
#define err_set_file my_err_set_file
#define err_set_exit my_err_set_exit
#define linux_err_dummy_for_coverage my_linux_err_dummy_for_coverage
#define linux_err_set_terminate_for_test my_linux_err_set_terminate_for_test
#define linux_err_reset_terminate_for_test my_linux_err_reset_terminate_for_test

#include "../src/err.c"

#undef ERR_DEFAULT_EXIT
#undef ERR_MOCK_VFPRINTF
#undef ERR_MOCK_FPRINTF
#undef vwarn
#undef vwarnx
#undef verr
#undef verrx
#undef err
#undef errx
#undef warn
#undef warnx
#undef vwarnc
#undef verrc
#undef warnc
#undef errc
#undef err_set_file
#undef err_set_exit
#undef linux_err_dummy_for_coverage
#undef linux_err_set_terminate_for_test
#undef linux_err_reset_terminate_for_test

#ifdef DEFINED_WIN32_FOR_TEST
#undef _WIN32
#endif

static void call_my_vwarn(const char *fmt, ...) LINUX_ERR_PRINTF(1, 2);
static void call_my_vwarnx(const char *fmt, ...) LINUX_ERR_PRINTF(1, 2);
static void call_my_verr(int eval, const char *fmt, ...) LINUX_ERR_PRINTF(2, 3);
static void call_my_verrx(int eval, const char *fmt, ...)
    LINUX_ERR_PRINTF(2, 3);
static void call_my_vwarnc(int code, const char *fmt, ...)
    LINUX_ERR_PRINTF(2, 3);
static void call_my_verrc(int eval, int code, const char *fmt, ...)
    LINUX_ERR_PRINTF(3, 4);

static void call_my_vwarn(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  my_vwarn(fmt, ap);
  va_end(ap);
}

static void call_my_vwarnx(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  my_vwarnx(fmt, ap);
  va_end(ap);
}

static void call_my_verr(int eval, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  my_verr(eval, fmt, ap);
  va_end(ap);
}

static void call_my_verrx(int eval, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  my_verrx(eval, fmt, ap);
  va_end(ap);
}

static void call_my_vwarnc(int code, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  my_vwarnc(code, fmt, ap);
  va_end(ap);
}

static void call_my_verrc(int eval, int code, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  my_verrc(eval, code, fmt, ap);
  va_end(ap);
}

#if (!defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&  \
     !defined(__NetBSD__)) ||                                                  \
    defined(_MSC_VER) || defined(_WIN32)
static void call_linked_vwarnc(int code, const char *fmt, ...)
    LINUX_ERR_PRINTF(2, 3);
static void call_linked_verrc(int eval, int code, const char *fmt, ...)
    LINUX_ERR_PRINTF(3, 4);

static void call_linked_vwarnc(int code, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vwarnc(code, fmt, ap);
  va_end(ap);
}

static void call_linked_verrc(int eval, int code, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verrc(eval, code, fmt, ap);
  va_end(ap);
}
#endif

#if defined(_MSC_VER) || defined(_WIN32)
static void call_linked_vwarn(const char *fmt, ...) LINUX_ERR_PRINTF(1, 2);
static void call_linked_vwarnx(const char *fmt, ...) LINUX_ERR_PRINTF(1, 2);
static void call_linked_verr(int eval, const char *fmt, ...)
    LINUX_ERR_PRINTF(2, 3);
static void call_linked_verrx(int eval, const char *fmt, ...)
    LINUX_ERR_PRINTF(2, 3);

static void call_linked_vwarn(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vwarn(fmt, ap);
  va_end(ap);
}

static void call_linked_vwarnx(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vwarnx(fmt, ap);
  va_end(ap);
}

static void call_linked_verr(int eval, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verr(eval, fmt, ap);
  va_end(ap);
}

static void call_linked_verrx(int eval, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verrx(eval, fmt, ap);
  va_end(ap);
}
#endif

TEST test_warn_and_file_stream(void) {
  void (*fn_my_warn)(const char *, ...) = my_warn;
  void (*fn_my_warnx)(const char *, ...) = my_warnx;
  void (*fn_my_warnc)(int, const char *, ...) = my_warnc;
  void (*fn_call_my_vwarnx)(const char *, ...) = call_my_vwarnx;
  void (*fn_call_my_vwarnc)(int, const char *, ...) = call_my_vwarnc;

  ASSERT_EQ(0, linux_err_dummy_for_coverage());
  ASSERT_EQ(0, my_linux_err_dummy_for_coverage());

  /* Default stderr stream (g_err_file == NULL) */
  my_err_set_file(NULL);
  g_mock_stderr_buf[0] = '\0';
  errno = ENOENT;
  my_warn("test %d", 123);
  ASSERT(strstr(g_mock_stderr_buf, "test 123:") != NULL);

  /* Explicit stream (g_err_file != NULL) */
  my_err_set_file(stderr);
  g_mock_stderr_buf[0] = '\0';
  errno = ENOENT;
  fn_my_warn(NULL);
  ASSERT(strlen(g_mock_stderr_buf) > 0);
  my_err_set_file(NULL);

  g_mock_stderr_buf[0] = '\0';
  my_warnx("test %d", 123);
  ASSERT_STR_EQ("test 123\n", g_mock_stderr_buf);

  g_mock_stderr_buf[0] = '\0';
  fn_my_warnx(NULL);
  ASSERT_STR_EQ("\n", g_mock_stderr_buf);

  g_mock_stderr_buf[0] = '\0';
  errno = ENOENT;
  call_my_vwarn("vwarn %d", 1);
  call_my_vwarnx("vwarnx %d", 2);
  fn_call_my_vwarnx(NULL);
  my_warnc(ENOENT, "warnc %d", 3);
  fn_my_warnc(ENOENT, NULL);
  call_my_vwarnc(ENOENT, "vwarnc %d", 4);
  fn_call_my_vwarnc(ENOENT, NULL);
  ASSERT(strlen(g_mock_stderr_buf) > 0);

  PASS();
}

TEST test_err_and_exit_callbacks(void) {
  /* First exercise default_err_terminate via ERR_DEFAULT_EXIT */
  my_linux_err_reset_terminate_for_test();
  my_err_set_exit(NULL);
  g_mock_exit_status = -1;
  errno = ENOENT;
  my_err(40, "err default");
  ASSERT_EQ(40, g_mock_exit_status);

  my_linux_err_set_terminate_for_test(mock_exit_return);

  /* Without custom exit callback (g_err_exit == NULL) */
  my_err_set_exit(NULL);
  g_mock_exit_status = -1;
  errno = ENOENT;
  my_err(41, "err %d", 1);
  ASSERT_EQ(41, g_mock_exit_status);

  g_mock_exit_status = -1;
  my_errx(42, "errx %d", 2);
  ASSERT_EQ(42, g_mock_exit_status);

  g_mock_exit_status = -1;
  my_errc(43, ENOENT, "errc %d", 3);
  ASSERT_EQ(43, g_mock_exit_status);

  g_mock_exit_status = -1;
  call_my_verr(44, "verr %d", 4);
  ASSERT_EQ(44, g_mock_exit_status);

  g_mock_exit_status = -1;
  call_my_verrx(45, "verrx %d", 5);
  ASSERT_EQ(45, g_mock_exit_status);

  g_mock_exit_status = -1;
  call_my_verrc(46, ENOENT, "verrc %d", 6);
  ASSERT_EQ(46, g_mock_exit_status);

  /* With custom exit callback (g_err_exit != NULL) for all 3 exit paths */
  my_err_set_exit(custom_exit_cb);

  g_custom_exit_called = 0;
  my_err(51, "err cb");
  ASSERT_EQ(51, g_custom_exit_called);

  g_custom_exit_called = 0;
  my_errx(52, "errx cb");
  ASSERT_EQ(52, g_custom_exit_called);

  g_custom_exit_called = 0;
  my_errc(53, ENOENT, "errc cb");
  ASSERT_EQ(53, g_custom_exit_called);

  my_err_set_exit(NULL);
  my_linux_err_reset_terminate_for_test();
  PASS();
}

TEST test_linux_err_linked_library_symbols(void) {
  linux_err_set_terminate_for_test(mock_exit_return);
  linux_err_reset_terminate_for_test();
#if (!defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&  \
     !defined(__NetBSD__)) ||                                                  \
    defined(_MSC_VER) || defined(_WIN32)
  (void)0;
  {
    void (*fn_warnc)(int, const char *, ...) = warnc;
#if defined(_MSC_VER) || defined(_WIN32)
    void (*fn_warn)(const char *, ...) = warn;
    void (*fn_warnx)(const char *, ...) = warnx;
#endif
    FILE *tmp = NULL;
#if defined(_MSC_VER)
    if (tmpfile_s(&tmp) != 0) {
      tmp = NULL;
    }
#else
    tmp = tmpfile();
#endif
    linux_err_set_terminate_for_test(mock_exit_return);
    if (tmp != NULL) {
      err_set_file(tmp);
    }
    warnc(ENOENT, "linked warnc %d", 1);
    fn_warnc(ENOENT, NULL);
    call_linked_vwarnc(ENOENT, "linked vwarnc %d", 2);
    err_set_file(NULL);
    fn_warnc(ENOENT, NULL);
    if (tmp != NULL) {
      err_set_file(tmp);
    }

    err_set_exit(custom_exit_cb);
    g_custom_exit_called = 0;
    g_mock_exit_status = -1;
    errc(61, ENOENT, "linked errc");
    ASSERT_EQ(61, g_custom_exit_called);
    ASSERT_EQ(61, g_mock_exit_status);

    g_custom_exit_called = 0;
    g_mock_exit_status = -1;
    call_linked_verrc(62, ENOENT, "linked verrc");
    ASSERT_EQ(62, g_custom_exit_called);
    ASSERT_EQ(62, g_mock_exit_status);

#if defined(_MSC_VER) || defined(_WIN32)
    errno = ENOENT;
    warn("linked warn %d", 1);
    fn_warn(NULL);
    warnx("linked warnx %d", 2);
    fn_warnx(NULL);
    call_linked_vwarn("linked vwarn");
    call_linked_vwarnx("linked vwarnx");

    g_custom_exit_called = 0;
    err(63, "linked err");
    ASSERT_EQ(63, g_custom_exit_called);

    g_custom_exit_called = 0;
    errx(64, "linked errx");
    ASSERT_EQ(64, g_custom_exit_called);

    g_custom_exit_called = 0;
    call_linked_verr(65, "linked verr");
    ASSERT_EQ(65, g_custom_exit_called);

    g_custom_exit_called = 0;
    call_linked_verrx(66, "linked verrx");
    ASSERT_EQ(66, g_custom_exit_called);
#endif

    err_set_exit(NULL);
    err_set_file(NULL);
    linux_err_reset_terminate_for_test();
    if (tmp != NULL) {
      fclose(tmp);
    }
  }
#endif
  PASS();
}

SUITE(err_suite) {
  RUN_TEST(test_warn_and_file_stream);
  RUN_TEST(test_err_and_exit_callbacks);
  RUN_TEST(test_linux_err_linked_library_symbols);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(err_suite);
  GREATEST_MAIN_END();
}
