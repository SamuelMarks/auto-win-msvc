#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-time.h"
/* clang-format on */

TEST test_time(void) {
  struct timespec ts;
  int res = clock_gettime(CLOCK_REALTIME, &ts);
  ASSERT_EQ(0, res);
  PASS();
}

TEST test_getitimer(void) { SKIP(); /* Generated stub for getitimer */ }

TEST test_gettimeofday(void) { SKIP(); /* Generated stub for gettimeofday */ }

TEST test_setitimer(void) { SKIP(); /* Generated stub for setitimer */ }

TEST test_utimes(void) { SKIP(); /* Generated stub for utimes */ }

TEST test_clock_gettime(void) { SKIP(); /* Generated stub for clock_gettime */ }

TEST test_nanosleep(void) { SKIP(); /* Generated stub for nanosleep */ }

TEST test_localtime_r(void) { SKIP(); /* Generated stub for localtime_r */ }

SUITE(suite_posix_time_core) {
  RUN_TEST(test_time);
  RUN_TEST(test_getitimer);
  RUN_TEST(test_gettimeofday);
  RUN_TEST(test_setitimer);
  RUN_TEST(test_utimes);
  RUN_TEST(test_clock_gettime);
  RUN_TEST(test_nanosleep);
  RUN_TEST(test_localtime_r);
}
