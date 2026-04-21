#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>

TEST test_pthread(void) {
  pthread_mutex_t m;
  if (pthread_mutex_init(&m, NULL) == 0) {
    pthread_mutex_destroy(&m);
    PASS();
  }
  SKIP();
}

TEST test_posix_pthread_atfork_prepare(void) {
  SKIP(); /* Generated stub for posix_pthread_atfork_prepare */
}

TEST test_posix_pthread_atfork_parent(void) {
  SKIP(); /* Generated stub for posix_pthread_atfork_parent */
}

TEST test_posix_pthread_atfork_child(void) {
  SKIP(); /* Generated stub for posix_pthread_atfork_child */
}

SUITE(suite_posix_pthread_pthread) {
  RUN_TEST(test_pthread);
  RUN_TEST(test_posix_pthread_atfork_prepare);
  RUN_TEST(test_posix_pthread_atfork_parent);
  RUN_TEST(test_posix_pthread_atfork_child);
}
