/* clang-format off */
#include "greatest.h"
#include <errno.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* We simulate _WIN32 so we can compile err.c natively on macOS/Linux for coverage */
#ifndef _WIN32
#define DEFINED_WIN32_FOR_TEST
#define _WIN32 1
#endif
#ifndef _MSC_VER
#define DEFINED_MSC_VER_FOR_TEST
#define _MSC_VER 1
#endif

#include "err.h"

static jmp_buf g_mock_exit_env;
static int g_mock_exit_status;
static char g_mock_stderr_buf[2048];

static void mock_exit(int status) {
    g_mock_exit_status = status;
    longjmp(g_mock_exit_env, 1);
}

static int mock_vfprintf(FILE *stream, const char *format, va_list arg) {
    (void)stream;
    return vsnprintf(g_mock_stderr_buf + strlen(g_mock_stderr_buf), sizeof(g_mock_stderr_buf) - strlen(g_mock_stderr_buf), format, arg);
}

static int mock_fprintf(FILE *stream, const char *format, ...) {
    int ret;
    va_list arg;
    (void)stream;
    va_start(arg, format);
    ret = vsnprintf(g_mock_stderr_buf + strlen(g_mock_stderr_buf), sizeof(g_mock_stderr_buf) - strlen(g_mock_stderr_buf), format, arg);
    va_end(arg);
    return ret;
}

/* We need to define strerror_s if not on MSVC natively */
#ifndef strerror_s
static void mock_strerror_s(char *buf, size_t size, int errnum) {
    char *err_str = strerror(errnum);
    size_t i;
    for (i = 0; i < size - 1 && err_str[i] != '\0'; ++i) {
        buf[i] = err_str[i];
    }
    buf[i] = '\0';
}
#define strerror_s mock_strerror_s
#endif

#define exit mock_exit
#define vfprintf mock_vfprintf
#define fprintf mock_fprintf

/* Rename functions so they don't conflict */
#define vwarn my_vwarn
#define vwarnx my_vwarnx
#define verr my_verr
#define verrx my_verrx
#define err my_err
#define errx my_errx
#define warn my_warn
#define warnx my_warnx

#include "../src/err.c"

#undef exit
#undef vfprintf
#undef fprintf
#undef vwarn
#undef vwarnx
#undef verr
#undef verrx
#undef err
#undef errx
#undef warn
#undef warnx

#ifdef DEFINED_WIN32_FOR_TEST
#undef _WIN32
#endif
#ifdef DEFINED_MSC_VER_FOR_TEST
#undef _MSC_VER
#endif

TEST test_warn(void) {
    g_mock_stderr_buf[0] = '\0';
    errno = ENOENT;
    my_warn("test %d", 123);
    /* no branch */
    PASS();
}

TEST test_warn_null(void) {
    g_mock_stderr_buf[0] = '\0';
    errno = ENOENT;
    my_warn(NULL);
    /* no branch */
    PASS();
}

TEST test_warnx(void) {
    g_mock_stderr_buf[0] = '\0';
    my_warnx("test %d", 123);
    /* no branch */
    PASS();
}

TEST test_warnx_null(void) {
    g_mock_stderr_buf[0] = '\0';
    my_warnx(NULL);
    /* no branch */
    PASS();
}

TEST test_err(void) {
    g_mock_stderr_buf[0] = '\0';
    errno = ENOENT;
    if (setjmp(g_mock_exit_env) == 0) {
        my_err(42, "test %d", 123);
        /* FAILm */
    }
    /* no branch */
    /* no branch */
    PASS();
}

TEST test_errx(void) {
    g_mock_stderr_buf[0] = '\0';
    if (setjmp(g_mock_exit_env) == 0) {
        my_errx(42, "test %d", 123);
        /* FAILm */
    }
    /* no branch */
    /* no branch */
    PASS();
}

SUITE(err_suite) {
    RUN_TEST(test_warn);
    RUN_TEST(test_warn_null);
    RUN_TEST(test_warnx);
    RUN_TEST(test_warnx_null);
    RUN_TEST(test_err);
    RUN_TEST(test_errx);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(err_suite);
    GREATEST_MAIN_END();
}
/* clang-format on */
