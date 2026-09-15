#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-time.h"
#include <stdio.h>
#include <string.h>
#if !defined(_WIN32)
#include <sys/time.h>
#endif
/* clang-format on */

TEST test_posix_time_get_info(void) {
  enum posix_time_error_code rc;
  int info;

  info = 0;
  rc = posix_time_get_info(NULL);
  if (rc != POSIX_TIME_ERROR_NULL_POINTER) {
    printf("Expected POSIX_TIME_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_time_get_info(&info);
  if (rc != POSIX_TIME_SUCCESS) {
    printf("posix_time_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_clock_gettime(void) {
  struct timespec ts;
  int res;

  /* CLOCK_REALTIME */
  memset(&ts, 0, sizeof(ts));
  res = clock_gettime(CLOCK_REALTIME, &ts);
  ASSERT_EQ(0, res);
  ASSERT(ts.tv_sec > 0);

  /* CLOCK_MONOTONIC */
  memset(&ts, 0, sizeof(ts));
  res = clock_gettime(CLOCK_MONOTONIC, &ts);
  ASSERT_EQ(0, res);

  /* Invalid clock id */
  res = clock_gettime(9999, &ts);
  ASSERT_EQ(-1, res);

  PASS();
}

TEST test_gettimeofday(void) {
  struct timeval tv;
  struct timezone tz;
  int res;

  /* Normal gettimeofday with tv and tz */
  memset(&tv, 0, sizeof(tv));
  memset(&tz, 0, sizeof(tz));
  res = gettimeofday(&tv, &tz);
  ASSERT_EQ(0, res);
  ASSERT(tv.tv_sec > 0);

  /* tv only */
  res = gettimeofday(&tv, NULL);
  ASSERT_EQ(0, res);

  /* tz only */
  res = gettimeofday(NULL, &tz);
  ASSERT_EQ(0, res);

  /* both null */
  res = gettimeofday(NULL, NULL);
  ASSERT_EQ(0, res);

  PASS();
}

TEST test_nanosleep(void) {
  struct timespec req;
  struct timespec rem;
  int res;

  /* Null request */
  res = nanosleep(NULL, NULL);
  ASSERT_EQ(-1, res);

  /* Valid short sleep: 1 millisecond = 1000000 nanoseconds */
  req.tv_sec = 0;
  req.tv_nsec = 1000000;
  memset(&rem, 0, sizeof(rem));
  res = nanosleep(&req, &rem);
  ASSERT_EQ(0, res);

  PASS();
}

TEST test_localtime_r(void) {
  time_t now;
  struct tm res_tm;
  struct tm *p;

  /* Valid check */
  now = time(NULL);
  memset(&res_tm, 0, sizeof(res_tm));
  p = localtime_r(&now, &res_tm);
  ASSERT_EQ(&res_tm, p);
  ASSERT(res_tm.tm_year > 100);

  PASS();
}

TEST test_getitimer_setitimer(void) {
  struct itimerval itv;
  struct itimerval old_itv;
  int res;

  /* Invalid timer type */
  res = getitimer(-1, &itv);
  ASSERT_EQ(-1, res);
  res = getitimer(99, &itv);
  ASSERT_EQ(-1, res);
  res = getitimer(ITIMER_REAL, NULL);
  ASSERT_EQ(-1, res);

  /* Valid getitimer */
  res = getitimer(ITIMER_REAL, &itv);
  ASSERT_EQ(0, res);

  /* Invalid setitimer */
  res = setitimer(-1, &itv, NULL);
  ASSERT_EQ(-1, res);

  /* Valid setitimer: zero to disarm */
  memset(&itv, 0, sizeof(itv));
  res = setitimer(ITIMER_REAL, &itv, &old_itv);
  ASSERT_EQ(0, res);

  PASS();
}

TEST test_utimes(void) {
  int res;
  struct timeval times[2];
  FILE *f;
  const char *test_path = "test_posix_time_tmp.txt";

  /* Invalid file */
  res = utimes(NULL, NULL);
  ASSERT_EQ(-1, res);

  res = utimes("non_existent_file_12345.xyz", NULL);
  ASSERT_EQ(-1, res);

  /* Create temporary file */
#if defined(_MSC_VER)
  if (fopen_s(&f, test_path, "w") != 0) {
    f = NULL;
  }
#else
  f = fopen(test_path, "w");
#endif
  ASSERT(f != NULL);
  fputs("hello", f);
  fclose(f);

  /* utimes with NULL times (sets to current time) */
  res = utimes(test_path, NULL);
  ASSERT_EQ(0, res);

  /* utimes with explicit times */
  times[0].tv_sec = 1000000;
  times[0].tv_usec = 0;
  times[1].tv_sec = 1000000;
  times[1].tv_usec = 0;
  res = utimes(test_path, times);
  ASSERT_EQ(0, res);

  remove(test_path);
  PASS();
}

SUITE(suite_posix_time_core) {
  RUN_TEST(test_posix_time_get_info);
  RUN_TEST(test_clock_gettime);
  RUN_TEST(test_gettimeofday);
  RUN_TEST(test_nanosleep);
  RUN_TEST(test_localtime_r);
  RUN_TEST(test_getitimer_setitimer);
  RUN_TEST(test_utimes);
}
