/* clang-format off */
#include "greatest.h"
#include "posix-core.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#if !defined(__DOS__) && !defined(__MSDOS__) && !defined(__WATCOMC__)
static int g_mock_asprintf_oom = 0;
static int g_mock_vsclen_fail = 0;

static void *mock_asprintf_malloc(size_t sz) {
  if (g_mock_asprintf_oom) {
    return NULL;
  }
  return malloc(sz);
}

static int mock_asprintf_vsclen(const char *fmt, va_list ap)
    POSIX_CORE_PRINTF_FORMAT(1, 0);

static int mock_asprintf_vsclen(const char *fmt, va_list ap) {
  if (g_mock_vsclen_fail) {
    return -1;
  }
#if defined(_MSC_VER)
  return _vscprintf(fmt, ap);
#else
  return vsnprintf(NULL, 0, fmt, ap);
#endif
}

int mocked_vasprintf(char **strp, const char *fmt, va_list ap)
    POSIX_CORE_PRINTF_FORMAT(2, 0);
int mocked_asprintf(char **strp, const char *fmt, ...)
    POSIX_CORE_PRINTF_FORMAT(2, 3);

#define POSIX_ASPRINTF_MOCK_malloc mock_asprintf_malloc
#define POSIX_ASPRINTF_MOCK_vsclen mock_asprintf_vsclen
#define vasprintf mocked_vasprintf
#define asprintf mocked_asprintf

#include "../src/posix_asprintf.c"

#undef vasprintf
#undef asprintf
#endif

TEST test_asprintf_and_vasprintf(void) {
  int (*volatile fn_asprintf)(char **, const char *, ...) = asprintf;
#if !defined(__DOS__) && !defined(__MSDOS__) && !defined(__WATCOMC__)
  int (*volatile fn_mocked_asprintf)(char **, const char *, ...) =
      mocked_asprintf;
#endif
  char *out = NULL;
  int len;

  /* Linked vasprintf & asprintf */
  ASSERT_EQ(-1, fn_asprintf(NULL, "abc %d", 1));
  ASSERT_EQ(-1, fn_asprintf(&out, NULL));

  len = asprintf(&out, "hello %s %d", "world", 42);
  ASSERT_EQ(14, len);
  ASSERT(out != NULL);
  ASSERT_STR_EQ("hello world 42", out);
  free(out);
  out = NULL;

#if !defined(__DOS__) && !defined(__MSDOS__) && !defined(__WATCOMC__)
  /* Mocked vasprintf & asprintf (including OOM and negative format length) */
  ASSERT_EQ(-1, fn_mocked_asprintf(NULL, "abc"));
  ASSERT_EQ(-1, fn_mocked_asprintf(&out, NULL));

  g_mock_vsclen_fail = 1;
  out = (char *)(size_t)1;
  ASSERT_EQ(-1, mocked_asprintf(&out, "len fail %d", 7));
  ASSERT(out == NULL);
  g_mock_vsclen_fail = 0;

  g_mock_asprintf_oom = 1;
  out = (char *)(size_t)1;
  ASSERT_EQ(-1, mocked_asprintf(&out, "oom %d", 9));
  ASSERT(out == NULL);
  g_mock_asprintf_oom = 0;

  len = mocked_asprintf(&out, "ok %d", 123);
  ASSERT_EQ(6, len);
  ASSERT(out != NULL);
  ASSERT_STR_EQ("ok 123", out);
  free(out);
#endif

  PASS();
}

TEST test_dprintf(void) {
  int (*volatile fn_dprintf)(int, const char *, ...) = dprintf;
  FILE *fp = NULL;
  int fd;
  int n;
  char read_buf[64];

  /* Error branch (ret < 0 from vasprintf when fmt == NULL) */
  ASSERT_EQ(-1, fn_dprintf(-1, NULL));

#if defined(_MSC_VER)
  if (fopen_s(&fp, "test_posix_dprintf_tmp.txt", "wb+") != 0) {
    fp = NULL;
  }
#else
  fp = fopen("test_posix_dprintf_tmp.txt", "wb+");
#endif
  ASSERT(fp != NULL);
#if defined(_WIN32)
  fd = _fileno(fp);
#else
  fd = fileno(fp);
#endif
  n = dprintf(fd, "dprintf %d %s", 99, "ok");
  ASSERT_EQ(13, n);

  fflush(fp);
  fseek(fp, 0, SEEK_SET);
  memset(read_buf, 0, sizeof(read_buf));
  ASSERT_EQ((size_t)13, fread(read_buf, 1, sizeof(read_buf) - 1, fp));
  fclose(fp);
  remove("test_posix_dprintf_tmp.txt");
  ASSERT_STR_EQ("dprintf 99 ok", read_buf);

  PASS();
}

TEST test_posix_core_header_inlines(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  int fd;

  /* 1. posix_core_open without _O_CREAT */
  fd = posix_core_open("nonexistent_posix_core_open_9999.tmp", _O_RDONLY);
  ASSERT_EQ(-1, fd);

  /* 2. posix_core_open with _O_CREAT and pmode == 0 ((pmode & _S_IWRITE) == 0)
   */
  fd = posix_core_open("test_pco_zero.tmp", _O_RDWR | _O_CREAT | _O_TRUNC, 0);
  ASSERT(fd >= 0);
  posix_close(fd);
  _unlink("test_pco_zero.tmp");

  /* 3. posix_core_open with _O_CREAT and writable pmode */
  fd = posix_core_open("test_pco_rw.tmp", _O_RDWR | _O_CREAT | _O_TRUNC,
                       _S_IREAD | _S_IWRITE);
  ASSERT(fd >= 0);
  posix_close(fd);
  _unlink("test_pco_rw.tmp");

  /* 4. posix_core_sleep & posix_core_usleep */
  ASSERT_EQ(0u, posix_core_sleep(0));
  ASSERT_EQ(0, posix_core_usleep(0));
#endif
  PASS();
}

SUITE(suite_posix_core_printf) {
  RUN_TEST(test_asprintf_and_vasprintf);
  RUN_TEST(test_dprintf);
  RUN_TEST(test_posix_core_header_inlines);
}
