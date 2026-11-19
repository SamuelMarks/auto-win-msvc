/* test_.c - Strict C89 Cleanup Handler Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>
/* clang-format on */

static int g_cleanup_ran;

static void cleanup_cb(void *arg) {
  int *p = (int *)arg;
  if (p) {
    (*p) += 10;
  }
  g_cleanup_ran++;
}

#if defined(_WIN32) && !defined(__CYGWIN__)
static unsigned long mock_tls_alloc_fail(void) { return 0xFFFFFFFFUL; }
#endif

TEST test__posix_pthread_cleanup_push(void) {
  int val = 1;
  g_cleanup_ran = 0;

  pthread_cleanup_push(cleanup_cb, &val);
  val += 2;
  pthread_cleanup_pop(1);

  ASSERT_EQ(13, val);
  ASSERT_EQ(1, g_cleanup_ran);

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct _pthread_cleanup_buffer buf;
    struct posix_pthread_win32_hooks hooks;

    _posix_pthread_cleanup_push(NULL, cleanup_cb, &val);

    posix_pthread_get_default_win32_hooks(&hooks);
    hooks.tls_alloc = mock_tls_alloc_fail;
    posix_pthread_set_win32_hooks(&hooks);
    _posix_pthread_cleanup_push(&buf, cleanup_cb, &val);
    _posix_pthread_cleanup_pop(&buf, 0);
    posix_pthread_set_win32_hooks(NULL);
  }
#endif
  PASS();
}

TEST test__posix_pthread_cleanup_pop(void) {
  int val = 5;
  g_cleanup_ran = 0;

  pthread_cleanup_push(cleanup_cb, &val);
  pthread_cleanup_pop(0);
  ASSERT_EQ(5, val);
  ASSERT_EQ(0, g_cleanup_ran);

#if defined(_WIN32) && !defined(__CYGWIN__)
  {
    struct _pthread_cleanup_buffer outer;
    struct _pthread_cleanup_buffer inner;
    struct _pthread_cleanup_buffer stray;

    _posix_pthread_cleanup_pop(NULL, 1);

    _posix_pthread_cleanup_push(&outer, NULL, NULL);
    _posix_pthread_cleanup_push(&inner, cleanup_cb, &val);

    /* Pop out-of-order node first (top != &outer), with NULL routine */
    stray.routine = NULL;
    stray.arg = NULL;
    stray.next = NULL;
    _posix_pthread_cleanup_pop(&stray, 1);

    /* Pop valid inner and outer */
    _posix_pthread_cleanup_pop(&inner, 1);
    _posix_pthread_cleanup_pop(&outer, 1);
    ASSERT_EQ(15, val);
  }
#endif
  PASS();
}

SUITE(suite_posix_pthread_) {
  RUN_TEST(test__posix_pthread_cleanup_push);
  RUN_TEST(test__posix_pthread_cleanup_pop);
}
