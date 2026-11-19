/* test_sched.c - Strict C89 Scheduling Polyfill Tests */
/* clang-format off */
#include "greatest.h"
#include "posix-pthread.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

TEST test_sched_get_priority_max(void) {
  ASSERT(sched_get_priority_max(SCHED_OTHER) >=
         sched_get_priority_min(SCHED_OTHER));
  ASSERT(sched_get_priority_max(SCHED_FIFO) >=
         sched_get_priority_min(SCHED_FIFO));
  ASSERT(sched_get_priority_max(SCHED_RR) >= sched_get_priority_min(SCHED_RR));
#if !defined(__APPLE__)
  ASSERT_EQ(-1, sched_get_priority_max(9999));
#endif
  PASS();
}

TEST test_sched_get_priority_min(void) {
  ASSERT(sched_get_priority_min(SCHED_OTHER) <=
         sched_get_priority_max(SCHED_OTHER));
  ASSERT(sched_get_priority_min(SCHED_FIFO) <=
         sched_get_priority_max(SCHED_FIFO));
  ASSERT(sched_get_priority_min(SCHED_RR) <= sched_get_priority_max(SCHED_RR));
#if !defined(__APPLE__)
  ASSERT_EQ(-1, sched_get_priority_min(9999));
#endif
  PASS();
}

TEST test_sched_getparam(void) {
#if !defined(__APPLE__)
  struct sched_param sp;
  ASSERT_EQ(0, sched_getparam(0, &sp));
  ASSERT_EQ(-1, sched_getparam(-1, &sp));
  ASSERT_EQ(-1, sched_getparam(0, NULL));
#endif
  PASS();
}

TEST test_sched_getscheduler(void) {
#if !defined(__APPLE__)
  ASSERT(sched_getscheduler(0) >= 0);
  ASSERT_EQ(-1, sched_getscheduler(-1));
#endif
  PASS();
}

TEST test_sched_rr_get_interval(void) {
#if !defined(__APPLE__)
  struct timespec ts;
  ASSERT_EQ(0, sched_rr_get_interval(0, &ts));
  ASSERT_EQ(-1, sched_rr_get_interval(-1, &ts));
  ASSERT_EQ(-1, sched_rr_get_interval(0, NULL));
#endif
  PASS();
}

TEST test_sched_setparam(void) {
#if !defined(__APPLE__)
  struct sched_param sp;
  ASSERT_EQ(0, sched_getparam(0, &sp));
  (void)sched_setparam(0, &sp);
  ASSERT_EQ(-1, sched_setparam(-1, &sp));
  ASSERT_EQ(-1, sched_setparam(0, NULL));
#endif
  PASS();
}

TEST test_sched_setscheduler(void) {
#if !defined(__APPLE__)
  struct sched_param sp;
  ASSERT_EQ(0, sched_getparam(0, &sp));
  (void)sched_setscheduler(0, SCHED_OTHER, &sp);
  ASSERT_EQ(-1, sched_setscheduler(-1, SCHED_OTHER, &sp));
  ASSERT_EQ(-1, sched_setscheduler(0, -1, &sp));
  ASSERT_EQ(-1, sched_setscheduler(0, SCHED_OTHER, NULL));
#endif
  PASS();
}

TEST test_sched_yield(void) {
  ASSERT_EQ(0, sched_yield());
  PASS();
}

SUITE(suite_posix_pthread_sched) {
  RUN_TEST(test_sched_get_priority_max);
  RUN_TEST(test_sched_get_priority_min);
  RUN_TEST(test_sched_getparam);
  RUN_TEST(test_sched_getscheduler);
  RUN_TEST(test_sched_rr_get_interval);
  RUN_TEST(test_sched_setparam);
  RUN_TEST(test_sched_setscheduler);
  RUN_TEST(test_sched_yield);
}
