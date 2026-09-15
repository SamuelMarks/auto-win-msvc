#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ucontext.h"
#include <stdio.h>
/* clang-format on */

#if defined(_WIN32)

static int g_fiber_ran = 0;
static ucontext_t g_main_ctx;
static ucontext_t g_fiber_ctx;

static void fiber_entry(void) {
  g_fiber_ran = 1;
  swapcontext(&g_fiber_ctx, &g_main_ctx);
}

#endif /* defined(_WIN32) */

TEST test_posix_ucontext_get_info(void) {
  enum posix_ucontext_error_code rc;
  int info;

  info = 0;
  rc = posix_ucontext_get_info(NULL);
  if (rc != POSIX_UCONTEXT_ERROR_NULL_POINTER) {
    printf("Expected POSIX_UCONTEXT_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_ucontext_get_info(&info);
  if (rc != POSIX_UCONTEXT_SUCCESS) {
    printf("posix_ucontext_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_ucontext_null_args(void) {
#if defined(_WIN32)
  ucontext_t ctx;

  ASSERT_EQ(-1, getcontext(NULL));
  ASSERT_EQ(-1, setcontext(NULL));
  ASSERT_EQ(-1, swapcontext(NULL, NULL));
  ASSERT_EQ(-1, swapcontext(&ctx, NULL));
  ASSERT_EQ(-1, swapcontext(NULL, &ctx));
#endif

  PASS();
}

TEST test_ucontext_switching(void) {
#if defined(_WIN32)
  char stack[16384];
  int ret;

  g_fiber_ran = 0;
  ret = getcontext(&g_fiber_ctx);
  if (ret == 0) {
    g_fiber_ctx.uc_stack.ss_sp = stack;
    g_fiber_ctx.uc_stack.ss_size = sizeof(stack);
    g_fiber_ctx.uc_link = &g_main_ctx;
    makecontext(&g_fiber_ctx, fiber_entry, 0);
    swapcontext(&g_main_ctx, &g_fiber_ctx);
    ASSERT_EQ(1, g_fiber_ran);
  }
#endif

  PASS();
}

SUITE(suite_posix_ucontext_core) {
  RUN_TEST(test_posix_ucontext_get_info);
  RUN_TEST(test_ucontext_null_args);
  RUN_TEST(test_ucontext_switching);
}
