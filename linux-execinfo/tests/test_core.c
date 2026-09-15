#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-execinfo.h"
#include <stdio.h>
#include <stdlib.h>
/* clang-format on */

TEST test_linux_execinfo_init(void) {
  enum linux_execinfo_error_code rc;
  int status;

  status = 0;
  rc = linux_execinfo_init(NULL);
  if (rc != LINUX_EXECINFO_ERROR_NULL_POINTER) {
    printf("Expected LINUX_EXECINFO_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_execinfo_init(&status);
  if (rc != LINUX_EXECINFO_SUCCESS) {
    printf("linux_execinfo_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_backtrace_functions(void) {
  void *buffer[16];
  int captured;
  char **symbols;
  error_type_t err;

  /* Invalid calls */
  err = backtrace(NULL, 0, NULL);
  ASSERT_EQ(-1, err);

  err = backtrace(NULL, 10, NULL);
  ASSERT_EQ(-1, err);

  err = backtrace(buffer, 0, NULL);
  ASSERT_EQ(-1, err);

  /* Valid call */
  captured = 0;
  err = backtrace(buffer, 16, &captured);
  ASSERT_EQ(ERR_NONE, err);

  /* Without captured pointer */
  err = backtrace(buffer, 16, NULL);
  ASSERT_EQ(ERR_NONE, err);

  /* Symbols invalid calls */
  symbols = backtrace_symbols(NULL, 0);
  ASSERT_EQ(NULL, symbols);

  symbols = backtrace_symbols(buffer, 0);
  ASSERT_EQ(NULL, symbols);

  /* Symbols valid call */
  symbols = backtrace_symbols(buffer, captured);
  if (symbols != NULL) {
    free(symbols);
  }

  /* Symbols fd invalid calls */
  err = backtrace_symbols_fd(NULL, 0, 2);
  ASSERT_EQ(-1, err);

  err = backtrace_symbols_fd(buffer, 0, 2);
  ASSERT_EQ(-1, err);

  /* Symbols fd valid call */
  if (captured > 0) {
    err = backtrace_symbols_fd(buffer, captured, 2);
    ASSERT_EQ(ERR_NONE, err);
  }

  PASS();
}

SUITE(suite_linux_execinfo_core) {
  RUN_TEST(test_linux_execinfo_init);
  RUN_TEST(test_backtrace_functions);
}
