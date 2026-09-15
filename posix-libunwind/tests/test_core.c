#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-libunwind.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

TEST test_posix_libunwind_get_info(void) {
  enum posix_libunwind_error_code rc;
  int info;

  info = 0;
  rc = posix_libunwind_get_info(NULL);
  if (rc != POSIX_LIBUNWIND_ERROR_NULL_POINTER) {
    printf("Expected POSIX_LIBUNWIND_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_libunwind_get_info(&info);
  if (rc != POSIX_LIBUNWIND_SUCCESS) {
    printf("posix_libunwind_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_unw_getcontext(void) {
  unw_context_t ctx;

  ASSERT_EQ(-1, unw_getcontext(NULL));
  ASSERT_EQ(0, unw_getcontext(&ctx));

  PASS();
}

TEST test_unw_init_local(void) {
  unw_context_t ctx;
  unw_cursor_t cur;

  ASSERT_EQ(0, unw_getcontext(&ctx));
  ASSERT_EQ(-1, unw_init_local(NULL, &ctx));
  ASSERT_EQ(-1, unw_init_local(&cur, NULL));
  ASSERT_EQ(0, unw_init_local(&cur, &ctx));

  PASS();
}

TEST test_unw_step(void) {
  unw_context_t ctx;
  unw_cursor_t cur;

  ASSERT_EQ(-1, unw_step(NULL));
  ASSERT_EQ(0, unw_getcontext(&ctx));
  ASSERT_EQ(0, unw_init_local(&cur, &ctx));
  ASSERT_EQ(0, unw_step(&cur));

  PASS();
}

TEST test_unw_get_reg(void) {
  unw_context_t ctx;
  unw_cursor_t cur;
  unw_word_t val;

  val = 123;
  ASSERT_EQ(0, unw_getcontext(&ctx));
  ASSERT_EQ(0, unw_init_local(&cur, &ctx));
  ASSERT_EQ(-1, unw_get_reg(NULL, 0, &val));
  ASSERT_EQ(-1, unw_get_reg(&cur, 0, NULL));
  ASSERT_EQ(0, unw_get_reg(&cur, 0, &val));
  ASSERT_EQ(0UL, val);

  PASS();
}

TEST test_unw_get_proc_name(void) {
  unw_context_t ctx;
  unw_cursor_t cur;
  char buf[32];
  unw_word_t off;

  off = 123;
  ASSERT_EQ(0, unw_getcontext(&ctx));
  ASSERT_EQ(0, unw_init_local(&cur, &ctx));

  ASSERT_EQ(-1, unw_get_proc_name(NULL, buf, sizeof(buf), &off));
  ASSERT_EQ(-1, unw_get_proc_name(&cur, NULL, sizeof(buf), &off));
  ASSERT_EQ(-1, unw_get_proc_name(&cur, buf, 0, &off));

  ASSERT_EQ(0, unw_get_proc_name(&cur, buf, sizeof(buf), NULL));
  ASSERT_STR_EQ("unknown", buf);

  ASSERT_EQ(0, unw_get_proc_name(&cur, buf, sizeof(buf), &off));
  ASSERT_STR_EQ("unknown", buf);
  ASSERT_EQ(0UL, off);

  PASS();
}

SUITE(suite_posix_libunwind_core) {
  RUN_TEST(test_posix_libunwind_get_info);
  RUN_TEST(test_unw_getcontext);
  RUN_TEST(test_unw_init_local);
  RUN_TEST(test_unw_step);
  RUN_TEST(test_unw_get_reg);
  RUN_TEST(test_unw_get_proc_name);
}
