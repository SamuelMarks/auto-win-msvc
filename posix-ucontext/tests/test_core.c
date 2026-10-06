#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ucontext.h"
#include <stdio.h>
/* clang-format on */

#if defined(_WIN32)

static int g_fiber_ran = 0;
static int g_fiber_args_ran = 0;
static ucontext_t g_main_ctx;
static ucontext_t g_fiber_ctx;
static ucontext_t g_fiber_ctx_args;

static void fiber_entry(void) {
  g_fiber_ran = 1;
  swapcontext(&g_fiber_ctx, &g_main_ctx);
}

static void fiber_entry_args(int a, int b) {
  if (a == 42 && b == 84) {
    g_fiber_args_ran = 1;
  }
  swapcontext(&g_fiber_ctx_args, &g_main_ctx);
}

#endif /* defined(_WIN32) */

TEST test_posix_ucontext_get_info(void) {
  auto_win_msvc_error_t rc;
  int info;

  info = 0;
  rc = posix_ucontext_get_info(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_ucontext_get_info(&info);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    printf("posix_ucontext_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_ucontext_null_args(void) {
#if defined(_WIN32)
  ucontext_t ctx;

  memset(&ctx, 0, sizeof(ctx));
  ASSERT_EQ(-1, getcontext(NULL));
  ASSERT_EQ(-1, setcontext(NULL));
  ASSERT_EQ(-1, setcontext(&ctx)); /* Invalid fiber */
  ASSERT_EQ(-1, swapcontext(NULL, NULL));
  ASSERT_EQ(-1, swapcontext(&ctx, NULL));
  ASSERT_EQ(-1, swapcontext(NULL, &ctx));
  ASSERT_EQ(-1, swapcontext(&g_main_ctx, &ctx)); /* Invalid fiber */
#endif

  PASS();
}

TEST test_ucontext_switching(void) {
#if defined(_WIN32)
  char stack[16384];
  char stack_args[16384];
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

  g_fiber_args_ran = 0;
  ret = getcontext(&g_fiber_ctx_args);
  if (ret == 0) {
    g_fiber_ctx_args.uc_stack.ss_sp = stack_args;
    g_fiber_ctx_args.uc_stack.ss_size = sizeof(stack_args);
    g_fiber_ctx_args.uc_link = &g_main_ctx;
    makecontext(&g_fiber_ctx_args, (void (*)(void))fiber_entry_args, 2, 42, 84);
    swapcontext(&g_main_ctx, &g_fiber_ctx_args);
    ASSERT_EQ(1, g_fiber_args_ran);
  }
#endif

  PASS();
}

SUITE(suite_posix_ucontext_core) {
  RUN_TEST(test_posix_ucontext_get_info);
  RUN_TEST(test_ucontext_null_args);
  RUN_TEST(test_ucontext_switching);
}
