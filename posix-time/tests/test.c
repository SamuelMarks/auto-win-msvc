/* test.c - 100% Test Coverage Stubs */
#include "greatest.h"
#include "posix-time.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER)
#pragma warning(disable : 4127) /* conditional expression is constant (for     \
                                   while(0) in greatest.h) */
#endif

TEST test_utime_null(void) {
  const char *test_file = "test_utime.txt";
  FILE *f = NULL;
#if defined(_MSC_VER)
  fopen_s(&f, test_file, "w");
#else
  f = fopen(test_file, "w");
#endif
  if (f) {
    fputs("test", f);
    fclose(f);
  }

  ASSERT_EQ(0, utime(test_file, NULL));
  remove(test_file);
  PASS();
}

TEST test_utime_values(void) {
  struct utimbuf times;
  const char *test_file = "test_utime_val.txt";
  FILE *f = NULL;
#if defined(_MSC_VER)
  fopen_s(&f, test_file, "w");
#else
  f = fopen(test_file, "w");
#endif
  if (f) {
    fputs("test", f);
    fclose(f);
  }

  times.actime = 1000000000;
  times.modtime = 1000000000;
  ASSERT_EQ(0, utime(test_file, &times));
  remove(test_file);
  PASS();
}

TEST test_tzset(void) {
  tzset();
  PASS();
}

TEST test_gettimeofday(void) {
#ifdef _WIN32
  struct timeval tv;
  struct timezone tz;

  ASSERT_EQ(0, gettimeofday(&tv, &tz));
  ASSERT(tv.tv_sec > 0);

  /* Test with NULLs */
  ASSERT_EQ(0, gettimeofday(NULL, NULL));
#endif
  PASS();
}

TEST test_utimes(void) {
#ifdef _WIN32
  struct timeval times[2];
  const char *test_file = "test_utimes.txt";
  FILE *f = NULL;
#if defined(_MSC_VER)
  fopen_s(&f, test_file, "w");
#else
  f = fopen(test_file, "w");
#endif
  if (f) {
    fputs("test", f);
    fclose(f);
  }

  times[0].tv_sec = 1000000000;
  times[0].tv_usec = 500000;
  times[1].tv_sec = 1000000000;
  times[1].tv_usec = 500000;

  ASSERT_EQ(0, utimes(test_file, times));
  ASSERT_EQ(0, utimes(test_file, NULL));

  ASSERT_EQ(-1, utimes("nonexistent_file_12345.txt", times));
  ASSERT_EQ(-1, utimes(NULL, times));

  remove(test_file);
#endif
  PASS();
}

TEST test_itimer(void) {
#ifdef _WIN32
  struct itimerval val, oval, getval;

  val.it_interval.tv_sec = 1;
  val.it_interval.tv_usec = 0;
  val.it_value.tv_sec = 2;
  val.it_value.tv_usec = 0;

  ASSERT_EQ(0, setitimer(ITIMER_REAL, &val, &oval));
  ASSERT_EQ(0, getitimer(ITIMER_REAL, &getval));
  ASSERT_EQ(1, getval.it_interval.tv_sec);
  ASSERT_EQ(2, getval.it_value.tv_sec);

  ASSERT_EQ(-1, setitimer(999, &val, NULL));
  ASSERT_EQ(-1, getitimer(999, &getval));
#endif
  PASS();
}

SUITE(posix_time_suite) {
  RUN_TEST(test_utime_null);
  RUN_TEST(test_utime_values);
  RUN_TEST(test_tzset);
  RUN_TEST(test_gettimeofday);
  RUN_TEST(test_utimes);
  RUN_TEST(test_itimer);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(posix_time_suite);
  GREATEST_MAIN_END();
}
