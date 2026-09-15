#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-backtrace.h"
#include <stdio.h>
/* clang-format on */

static int g_callback_called = 0;

static int dummy_full_callback(void *data, uintptr_t pc, const char *filename,
                               int lineno, const char *function) {
  (void)data;
  (void)pc;
  (void)filename;
  (void)lineno;
  (void)function;
  g_callback_called = 1;
  return 0;
}

TEST test_linux_backtrace_init(void) {
  enum linux_backtrace_error_code rc;
  int status;

  status = 0;
  rc = linux_backtrace_init(NULL);
  if (rc != LINUX_BACKTRACE_ERROR_NULL_POINTER) {
    printf("Expected LINUX_BACKTRACE_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_backtrace_init(&status);
  if (rc != LINUX_BACKTRACE_SUCCESS) {
    printf("linux_backtrace_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_backtrace_create_state_and_pcinfo(void) {
  struct backtrace_state *state;
  int ret;

  state = backtrace_create_state("test_app", 1, NULL, NULL);
  ASSERT(state != NULL);

  g_callback_called = 0;
  ret = backtrace_pcinfo(state, (uintptr_t)0x1234, dummy_full_callback, NULL,
                         NULL);
  ASSERT_EQ(0, ret);
  ASSERT_EQ(1, g_callback_called);

  ret = backtrace_pcinfo(state, (uintptr_t)0x1234, NULL, NULL, NULL);
  ASSERT_EQ(0, ret);

  PASS();
}

SUITE(suite_linux_backtrace_core) {
  RUN_TEST(test_linux_backtrace_init);
  RUN_TEST(test_backtrace_create_state_and_pcinfo);
}
