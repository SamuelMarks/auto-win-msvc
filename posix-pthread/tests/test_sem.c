#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>

TEST test_sem_close(void) { SKIP(); /* Generated stub for sem_close */ }

TEST test_sem_destroy(void) { SKIP(); /* Generated stub for sem_destroy */ }

TEST test_sem_getvalue(void) { SKIP(); /* Generated stub for sem_getvalue */ }

TEST test_sem_init(void) { SKIP(); /* Generated stub for sem_init */ }

TEST test_sem_open(void) { SKIP(); /* Generated stub for sem_open */ }

TEST test_sem_post(void) { SKIP(); /* Generated stub for sem_post */ }

TEST test_sem_timedwait(void) { SKIP(); /* Generated stub for sem_timedwait */ }

TEST test_sem_trywait(void) { SKIP(); /* Generated stub for sem_trywait */ }

TEST test_sem_unlink(void) { SKIP(); /* Generated stub for sem_unlink */ }

TEST test_sem_wait(void) { SKIP(); /* Generated stub for sem_wait */ }

SUITE(suite_posix_pthread_sem) {
  RUN_TEST(test_sem_close);
  RUN_TEST(test_sem_destroy);
  RUN_TEST(test_sem_getvalue);
  RUN_TEST(test_sem_init);
  RUN_TEST(test_sem_open);
  RUN_TEST(test_sem_post);
  RUN_TEST(test_sem_timedwait);
  RUN_TEST(test_sem_trywait);
  RUN_TEST(test_sem_unlink);
  RUN_TEST(test_sem_wait);
}
