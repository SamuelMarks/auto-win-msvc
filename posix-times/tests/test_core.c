/* clang-format off */
#include "greatest.h"
#include "posix-times.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)
static int g_mock_times_ok = 1;

static int __stdcall mock_times_GetProcessTimes(void *hProc, void *cTime,
                                                void *eTime, void *kTime,
                                                void *uTime) {
  (void)hProc;
  if (g_mock_times_ok) {
    memset(cTime, 0, 8);
    memset(eTime, 0, 8);
    memset(kTime, 0, 8);
    memset(uTime, 0, 8);
    return 1;
  }
  return 0;
}

#define POSIX_TIMES_MOCK_GetProcessTimes mock_times_GetProcessTimes
#define posix_times mocked_posix_times
#define posix_times_get_info mocked_posix_times_get_info
#include "../src/posix-times.c"
#undef posix_times
#undef posix_times_get_info
#endif

TEST test_posix_times_get_info(void) {
  int info = 0;

  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, posix_times_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, posix_times_get_info(&info));
  ASSERT_EQ(1, info);

#if defined(_MSC_VER) || defined(_WIN32)
  info = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT,
            mocked_posix_times_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, mocked_posix_times_get_info(&info));
  ASSERT_EQ(1, info);
#endif

  PASS();
}

TEST test_times(void) {
#if defined(_MSC_VER) || defined(_WIN32)
  struct tms t;

  ASSERT_EQ((clock_t)0, times(NULL));
  ASSERT_EQ((clock_t)0, times(&t));

  ASSERT_EQ((clock_t)0, mocked_posix_times(NULL));

  g_mock_times_ok = 1;
  ASSERT_EQ((clock_t)0, mocked_posix_times(&t));
  ASSERT_EQ((clock_t)0, t.tms_stime);

  g_mock_times_ok = 0;
  t.tms_stime = (clock_t)99;
  ASSERT_EQ((clock_t)0, mocked_posix_times(&t));
  ASSERT_EQ((clock_t)0, t.tms_stime);
  ASSERT_EQ((clock_t)0, t.tms_utime);
#else
  struct tms t;
  (void)times(&t);
#endif
  PASS();
}

SUITE(suite_posix_times_core) {
  RUN_TEST(test_posix_times_get_info);
  RUN_TEST(test_times);
}
