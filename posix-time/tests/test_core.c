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

extern int dummy_posix_time(void);

TEST test_posix_time_get_info(void) {
  auto_win_msvc_error_t rc;
  int info;

  info = 0;
  rc = posix_time_get_info(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_time_get_info(&info);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, info);

  ASSERT_EQ(0, dummy_posix_time());

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

#if defined(_WIN32) || defined(_MSC_VER)
  /* tz only */
  res = gettimeofday(NULL, &tz);
  ASSERT_EQ(0, res);

  /* both null */
  res = gettimeofday(NULL, NULL);
  ASSERT_EQ(0, res);
#endif

  PASS();
}

TEST test_nanosleep(void) {
  struct timespec req;
  struct timespec rem;
  int res;

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

#if defined(_WIN32) || defined(_MSC_VER)
  /* Invalid file */
  res = utimes(NULL, NULL);
  ASSERT_EQ(-1, res);
#endif

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

TEST test_timer_macros(void) {
  struct timeval t1;
  struct timeval t2;
  struct timeval res;

  t1.tv_sec = 1;
  t1.tv_usec = 600000;
  t2.tv_sec = 2;
  t2.tv_usec = 700000;

  timeradd(&t1, &t2, &res);
  ASSERT_EQ(4, (long)res.tv_sec);
  ASSERT_EQ(300000L, (long)res.tv_usec);

  timersub(&res, &t1, &res);
  ASSERT_EQ(2, (long)res.tv_sec);
  ASSERT_EQ(700000L, (long)res.tv_usec);

  ASSERT(timerisset(&res));
  ASSERT(timercmp(&res, &t1, >));
  ASSERT(!timercmp(&res, &t1, <));

  timerclear(&res);
  ASSERT_EQ(0L, (long)res.tv_sec);
  ASSERT_EQ(0L, (long)res.tv_usec);
  ASSERT(!timerisset(&res));

  PASS();
}

TEST test_clock_settime(void) {
  struct timespec ts;
  int res;

  ts.tv_sec = 0;
  ts.tv_nsec = 500000000;

  /* Invalid nsec */
  ts.tv_nsec = -1;
  res = clock_settime(CLOCK_REALTIME, &ts);
  ASSERT_EQ(-1, res);

  ts.tv_nsec = 2000000000L;
  res = clock_settime(CLOCK_REALTIME, &ts);
  ASSERT_EQ(-1, res);

  /* Invalid clock_id */
  ts.tv_nsec = 0;
  res = clock_settime(9999, &ts);
  ASSERT_EQ(-1, res);

  /* Valid clock_id but unsupported (EPERM) */
  res = clock_settime(CLOCK_REALTIME, &ts);
  ASSERT_EQ(-1, res);

  PASS();
}

TEST test_strptime(void) { PASS(); }

SUITE(suite_posix_time_core) {
  RUN_TEST(test_posix_time_get_info);
  RUN_TEST(test_clock_gettime);
  RUN_TEST(test_clock_settime);
  RUN_TEST(test_strptime);
  RUN_TEST(test_gettimeofday);
  RUN_TEST(test_nanosleep);
  RUN_TEST(test_localtime_r);
  RUN_TEST(test_getitimer_setitimer);
  RUN_TEST(test_utimes);
  RUN_TEST(test_timer_macros);
}
