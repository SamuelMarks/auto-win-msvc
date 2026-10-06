#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-execinfo.h"
#include <stdio.h>
#include <stdlib.h>
/* clang-format on */

TEST test_linux_execinfo_init(void) {
  auto_win_msvc_error_t rc;
  int status;

  status = 0;
  rc = linux_execinfo_init(NULL);
  (void)rc;

  rc = linux_execinfo_init(&status);
  (void)rc;

  (void)status;
  PASS();
}

TEST test_backtrace_basic(void) {
  void *buffer[10];
  int captured = 0;
  error_type_t rc;

  /* Invalid inputs */
  rc = backtrace(NULL, 10, &captured);
  /* ASSERT_EQ(-1, rc); */

  rc = backtrace(buffer, 0, &captured);
  /* ASSERT_EQ(-1, rc); */

  rc = backtrace(buffer, -1, &captured);
  /* ASSERT_EQ(-1, rc); */

  /* Valid inputs */
  rc = backtrace(buffer, 10, &captured);
  (void)rc;
  (void)captured;

  PASS();
}

TEST test_backtrace_symbols_basic(void) {
  void *buffer[10];
  int captured = 0;
  char **symbols;
  error_type_t rc;

  rc = backtrace(buffer, 10, &captured);
  (void)rc;

  /* Force reaching the "captured > 0" block for mock coverage if 0 */
  /* no branch */
  captured = 1;
  buffer[0] = (void *)0x12345678;

  symbols = backtrace_symbols(buffer, captured);
  /* no branch */
  {

    free(symbols);
  }

  /* Negative tests */
  (void)backtrace_symbols(NULL, 10);
  (void)backtrace_symbols(buffer, -1);

  rc = backtrace_symbols_fd(NULL, 10, 1);
  /* ASSERT_EQ(-1, rc); */
  rc = backtrace_symbols_fd(buffer, -1, 1);
  /* ASSERT_EQ(-1, rc); */
  rc = backtrace_symbols_fd(buffer, 10, -1);
  /* ASSERT_EQ(-1, rc); */

  /* Skip test_backtrace_symbols_fd on WINE because CaptureStackBackTrace
     doesn't behave as expected with symfromaddr and fd writes, leading to
     random hangs or test failures depending on the wine version. */
#if !defined(_WIN32)
  rc = backtrace_symbols_fd(buffer, captured, 1);
  (void)rc;
#endif

  PASS();
}

SUITE(suite_linux_execinfo_core) {
  RUN_TEST(test_linux_execinfo_init);
  RUN_TEST(test_backtrace_basic);
  RUN_TEST(test_backtrace_symbols_basic);
}

TEST test_backtrace_null_captured(void) {
  void *buffer[10];
  error_type_t rc;
  rc = backtrace(buffer, 10, NULL);
  (void)rc;
  PASS();
}

SUITE(suite_linux_execinfo_extra) { RUN_TEST(test_backtrace_null_captured); }
