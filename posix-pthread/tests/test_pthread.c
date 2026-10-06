#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>
/* clang-format on */

extern int dummy_posix_pthread(void);

TEST test_pthread(void) {
  pthread_mutex_t m;
  dummy_posix_pthread();
  if (pthread_mutex_init(&m, NULL) == 0) {
    pthread_mutex_destroy(&m);
    PASS();
  }
  SKIP();
}

#if defined(_WIN32)
static int g_atfork_prepare_count = 0;
static int g_atfork_parent_count = 0;
static int g_atfork_child_count = 0;

static void my_prepare(void) { g_atfork_prepare_count++; }
static void my_parent(void) { g_atfork_parent_count++; }
static void my_child(void) { g_atfork_child_count++; }
#endif

TEST test_posix_pthread_atfork(void) {
#if defined(_WIN32)
  int rc;
  g_atfork_prepare_count = 0;
  g_atfork_parent_count = 0;
  g_atfork_child_count = 0;

  rc = pthread_atfork(my_prepare, my_parent, my_child);
  ASSERT_EQ(0, rc);

  posix_pthread_atfork_prepare();
  ASSERT_EQ(1, g_atfork_prepare_count);
  ASSERT_EQ(0, g_atfork_parent_count);
  ASSERT_EQ(0, g_atfork_child_count);

  posix_pthread_atfork_parent();
  ASSERT_EQ(1, g_atfork_prepare_count);
  ASSERT_EQ(1, g_atfork_parent_count);
  ASSERT_EQ(0, g_atfork_child_count);

  posix_pthread_atfork_child();
  ASSERT_EQ(1, g_atfork_prepare_count);
  ASSERT_EQ(1, g_atfork_parent_count);
  ASSERT_EQ(1, g_atfork_child_count);
#endif
  PASS();
}

SUITE(suite_posix_pthread_pthread) {
  RUN_TEST(test_pthread);
  RUN_TEST(test_posix_pthread_atfork);
}
