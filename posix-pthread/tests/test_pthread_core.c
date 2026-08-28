#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <stddef.h>
/* clang-format on */

TEST test_pthread_atfork(void) {
  SKIP(); /* Generated stub for pthread_atfork */
}

TEST test_pthread_cancel(void) {
  SKIP(); /* Generated stub for pthread_cancel */
}

TEST test_pthread_create(void) {
  SKIP(); /* Generated stub for pthread_create */
}

TEST test_pthread_detach(void) {
  SKIP(); /* Generated stub for pthread_detach */
}

TEST test_pthread_equal(void) { SKIP(); /* Generated stub for pthread_equal */ }

TEST test_pthread_exit(void) { SKIP(); /* Generated stub for pthread_exit */ }

TEST test_pthread_getconcurrency(void) {
  SKIP(); /* Generated stub for pthread_getconcurrency */
}

TEST test_pthread_getcpuclockid(void) {
  SKIP(); /* Generated stub for pthread_getcpuclockid */
}

TEST test_pthread_getschedparam(void) {
  SKIP(); /* Generated stub for pthread_getschedparam */
}

TEST test_pthread_getspecific(void) {
  SKIP(); /* Generated stub for pthread_getspecific */
}

TEST test_pthread_join(void) { SKIP(); /* Generated stub for pthread_join */ }

TEST test_pthread_once(void) { SKIP(); /* Generated stub for pthread_once */ }

TEST test_pthread_self(void) { SKIP(); /* Generated stub for pthread_self */ }

TEST test_pthread_setcancelstate(void) {
  SKIP(); /* Generated stub for pthread_setcancelstate */
}

TEST test_pthread_setcanceltype(void) {
  SKIP(); /* Generated stub for pthread_setcanceltype */
}

TEST test_pthread_setconcurrency(void) {
  SKIP(); /* Generated stub for pthread_setconcurrency */
}

TEST test_pthread_setschedparam(void) {
  SKIP(); /* Generated stub for pthread_setschedparam */
}

TEST test_pthread_setschedprio(void) {
  SKIP(); /* Generated stub for pthread_setschedprio */
}

TEST test_pthread_setspecific(void) {
  SKIP(); /* Generated stub for pthread_setspecific */
}

TEST test_pthread_setname_np(void) {
  SKIP(); /* Generated stub for pthread_setname_np */
}

TEST test_pthread_sigmask(void) {
  SKIP(); /* Generated stub for pthread_sigmask */
}

TEST test_pthread_testcancel(void) {
  SKIP(); /* Generated stub for pthread_testcancel */
}

SUITE(suite_posix_pthread_pthread_core) {
  RUN_TEST(test_pthread_atfork);
  RUN_TEST(test_pthread_cancel);
  RUN_TEST(test_pthread_create);
  RUN_TEST(test_pthread_detach);
  RUN_TEST(test_pthread_equal);
  RUN_TEST(test_pthread_exit);
  RUN_TEST(test_pthread_getconcurrency);
  RUN_TEST(test_pthread_getcpuclockid);
  RUN_TEST(test_pthread_getschedparam);
  RUN_TEST(test_pthread_getspecific);
  RUN_TEST(test_pthread_join);
  RUN_TEST(test_pthread_once);
  RUN_TEST(test_pthread_self);
  RUN_TEST(test_pthread_setcancelstate);
  RUN_TEST(test_pthread_setcanceltype);
  RUN_TEST(test_pthread_setconcurrency);
  RUN_TEST(test_pthread_setschedparam);
  RUN_TEST(test_pthread_setschedprio);
  RUN_TEST(test_pthread_setspecific);
  RUN_TEST(test_pthread_setname_np);
  RUN_TEST(test_pthread_sigmask);
  RUN_TEST(test_pthread_testcancel);
}
