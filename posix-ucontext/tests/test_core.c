/* clang-format off */
#include "greatest.h"
#include "posix-ucontext.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#if defined(_WIN32) && !defined(__CYGWIN__)

typedef unsigned long(__stdcall *TEST_THREAD_START)(void *);
__declspec(dllimport) void *__stdcall
CreateThread(void *lpThreadAttributes, size_t dwStackSize,
             TEST_THREAD_START lpStartAddress, void *lpParameter,
             unsigned long dwCreationFlags, unsigned long *lpThreadId);
__declspec(dllimport) unsigned long __stdcall
WaitForSingleObject(void *hHandle, unsigned long dwMilliseconds);
__declspec(dllimport) int __stdcall CloseHandle(void *hObject);

extern void posix_ucontext_set_hooks(void *(*malloc_fn)(size_t),
                                     void *(*get_fiber_fn)(void));
extern void posix_ucontext_reset_hooks(void);

static void *g_mock_fiber_val = NULL;
static void *mock_malloc_null(size_t sz) {
  (void)sz;
  return NULL;
}
static void *mock_get_fiber(void) { return g_mock_fiber_val; }

static int g_arg_sum = 0;
static ucontext_t g_main_ctx;
static ucontext_t g_chain_ctx[18];
static int g_worker_ran = 0;

static void fn_0(void) { g_arg_sum += 1; }
static void fn_1(int a) { g_arg_sum += a; }
static void fn_2(int a, int b) { g_arg_sum += a + b; }
static void fn_3(int a, int b, int c) { g_arg_sum += a + b + c; }
static void fn_4(int a, int b, int c, int d) { g_arg_sum += a + b + c + d; }
static void fn_5(int a, int b, int c, int d, int e) {
  g_arg_sum += a + b + c + d + e;
}
static void fn_6(int a, int b, int c, int d, int e, int f) {
  g_arg_sum += a + b + c + d + e + f;
}
static void fn_7(int a, int b, int c, int d, int e, int f, int g) {
  g_arg_sum += a + b + c + d + e + f + g;
}
static void fn_8(int a, int b, int c, int d, int e, int f, int g, int h) {
  g_arg_sum += a + b + c + d + e + f + g + h;
}

static void fn_worker_exit(void) { g_worker_ran = 1; }

static unsigned long __stdcall worker_thread_proc(void *param) {
  ucontext_t wctx;
  (void)param;
  memset(&wctx, 0, sizeof(wctx));
  if (getcontext(&wctx) == 0) {
    wctx.uc_link = NULL;
    wctx.uc_stack.ss_size = 0; /* exercises default 65536 stack_size */
    makecontext(&wctx, fn_worker_exit, 0);
    setcontext(&wctx);
  }
  return 0;
}

#endif

TEST test_posix_ucontext_get_info(void) {
  int avail = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT,
            posix_ucontext_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, posix_ucontext_get_info(&avail));
  ASSERT_EQ(1, avail);
  PASS();
}

TEST test_ucontext_null_and_invalid_args(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  ucontext_t ctx;
  ucontext_t empty_ctx;

  memset(&ctx, 0, sizeof(ctx));
  memset(&empty_ctx, 0, sizeof(empty_ctx));

  ASSERT_EQ(-1, getcontext(NULL));
  ASSERT_EQ(-1, setcontext(NULL));
  ASSERT_EQ(-1, setcontext(&empty_ctx));
  ASSERT_EQ(-1, swapcontext(NULL, &ctx));
  ASSERT_EQ(-1, swapcontext(&ctx, NULL));
  ASSERT_EQ(-1, swapcontext(&ctx, &empty_ctx));

  makecontext(NULL, fn_0, 0);
  makecontext(&ctx, fn_0, -1);
  makecontext(&ctx, fn_0, 9);

  /* OOM on makecontext and both 0x1e00 and NULL fiber probe paths */
  posix_ucontext_set_hooks(mock_malloc_null, mock_get_fiber);
  makecontext(&ctx, fn_0, 0);

  g_mock_fiber_val = (void *)(size_t)0x1e00;
  ASSERT_EQ(0, getcontext(&ctx));

  g_mock_fiber_val = NULL;
  ASSERT_EQ(0, getcontext(&ctx));

  posix_ucontext_reset_hooks();

  /* SwitchToFiber to current fiber returns immediately and covers setcontext's
   * return 0 */
  ASSERT_EQ(0, getcontext(&ctx));
  ASSERT_EQ(0, setcontext(&ctx));
#endif
  PASS();
}

TEST test_ucontext_switching(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  ucontext_t fctx;
  volatile int i;
  void *hthread;

  g_arg_sum = 0;

  /* Test argc = 0..8 */
  for (i = 0; i <= 8; ++i) {
    memset(&fctx, 0, sizeof(fctx));
    ASSERT_EQ(0, getcontext(&fctx));
    fctx.uc_stack.ss_size = 65536;
    fctx.uc_link = &g_main_ctx;
    switch (i) {
    case 0:
      makecontext(&fctx, fn_0, 0);
      break;
    case 1:
      makecontext(&fctx, (void (*)(void))fn_1, 1, 10);
      break;
    case 2:
      makecontext(&fctx, (void (*)(void))fn_2, 2, 1, 2);
      break;
    case 3:
      makecontext(&fctx, (void (*)(void))fn_3, 3, 1, 2, 3);
      break;
    case 4:
      makecontext(&fctx, (void (*)(void))fn_4, 4, 1, 2, 3, 4);
      break;
    case 5:
      makecontext(&fctx, (void (*)(void))fn_5, 5, 1, 2, 3, 4, 5);
      break;
    case 6:
      makecontext(&fctx, (void (*)(void))fn_6, 6, 1, 2, 3, 4, 5, 6);
      break;
    case 7:
      makecontext(&fctx, (void (*)(void))fn_7, 7, 1, 2, 3, 4, 5, 6, 7);
      break;
    default:
      makecontext(&fctx, (void (*)(void))fn_8, 8, 1, 2, 3, 4, 5, 6, 7, 8);
      break;
    }
    ASSERT_EQ(0, swapcontext(&g_main_ctx, &fctx));
  }
  ASSERT_EQ(1 + 10 + 3 + 6 + 10 + 15 + 21 + 28 + 36, g_arg_sum);

  /* Chain 18 fibers without intermediate cleanup so num_dead_fibers exceeds
   * MAX_DEAD_FIBERS (16), exercising both < 16 and >= 16 branches */
  for (i = 0; i < 18; ++i) {
    memset(&g_chain_ctx[i], 0, sizeof(g_chain_ctx[i]));
    ASSERT_EQ(0, getcontext(&g_chain_ctx[i]));
    g_chain_ctx[i].uc_stack.ss_size = 32768;
    g_chain_ctx[i].uc_link = (i + 1 < 18) ? &g_chain_ctx[i + 1] : &g_main_ctx;
    makecontext(&g_chain_ctx[i], fn_0, 0);
  }
  /* Pre-create all 18 fibers? Note makecontext calls cleanup_dead_fibers, so
   * none are dead yet; now swapcontext runs all 18 in a single chain! */
  ASSERT_EQ(0, swapcontext(&g_main_ctx, &g_chain_ctx[0]));
  /* Trigger cleanup of the 16 recorded dead fibers */
  makecontext(NULL, fn_0, 0);

  /* Test uc_link == NULL + default stack_size == 0 + ExitThread(0) on a
   * dedicated worker thread */
  g_worker_ran = 0;
  hthread = CreateThread(NULL, 0, worker_thread_proc, NULL, 0, NULL);
  ASSERT_NEQ(NULL, hthread);
  WaitForSingleObject(hthread, 5000UL);
  CloseHandle(hthread);
  ASSERT_EQ(1, g_worker_ran);
#endif

  PASS();
}

SUITE(suite_posix_ucontext_core) {
  RUN_TEST(test_posix_ucontext_get_info);
  RUN_TEST(test_ucontext_null_and_invalid_args);
  RUN_TEST(test_ucontext_switching);
}
