/* clang-format off */
#include "greatest.h"
#include <posix-ucontext.h>
#include <stdio.h>
/* clang-format on */

#if defined(_WIN32) && !defined(__CYGWIN__)
static ucontext_t uctx_main, uctx_func1, uctx_func2;
static int test_flag = 0;

static void func1(void) {
  test_flag = 1;
  swapcontext(&uctx_func1, &uctx_func2);
  test_flag = 3;
}

static void func2(void) {
  test_flag = 2;
  swapcontext(&uctx_func2, &uctx_func1);
  test_flag = 4;
}

static void func_with_args(int a, int b) { test_flag = a + b; }
#endif

TEST test_ucontext_basic(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  int ret;

  /* Just getcontext and setcontext. We can't really setcontext
     without causing an infinite loop if we just capture and restore. */
  ret = getcontext(&uctx_main);
  ASSERT_EQ(0, ret);
#endif
  PASS();
}

TEST test_ucontext_swap(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  test_flag = 0;

  getcontext(&uctx_func1);
  uctx_func1.uc_stack.ss_size = 65536;
  uctx_func1.uc_link = &uctx_main;
  makecontext(&uctx_func1, func1, 0);

  getcontext(&uctx_func2);
  uctx_func2.uc_stack.ss_size = 65536;
  uctx_func2.uc_link = &uctx_main;
  makecontext(&uctx_func2, func2, 0);

  /* Start func1 */
  swapcontext(&uctx_main, &uctx_func1);

  /* func1 runs, sets flag=1, swaps to func2 */
  /* func2 runs, sets flag=2, swaps to func1 */
  /* func1 resumes, sets flag=3, then returns to uc_link (main) */
  ASSERT_EQ(3, test_flag);
#endif
  PASS();
}

TEST test_ucontext_args(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  test_flag = 0;

  getcontext(&uctx_func1);
  uctx_func1.uc_stack.ss_size = 65536;
  uctx_func1.uc_link = &uctx_main;
  makecontext(&uctx_func1, (void (*)(void))func_with_args, 2, 10, 20);

  swapcontext(&uctx_main, &uctx_func1);

  ASSERT_EQ(30, test_flag);
#endif
  PASS();
}

SUITE(main_suite) {
  RUN_TEST(test_ucontext_basic);
  RUN_TEST(test_ucontext_swap);
  RUN_TEST(test_ucontext_args);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  (void)argc;
  (void)argv;
  RUN_SUITE(main_suite);
  GREATEST_MAIN_END();
}
