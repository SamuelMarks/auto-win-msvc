/* clang-format off */
#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE 1
#endif
#include "greatest.h"
#include "bsd-pthread-np.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#elif defined(__linux__)
#include <sched.h>
#endif
/* clang-format on */

TEST test_bsd_pthread_np_init(void) {
  auto_win_msvc_error_t rc;
  int status = 0;

  rc = bsd_pthread_np_init(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = bsd_pthread_np_init(&status);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, status);
  PASS();
}

TEST test_pthread_setaffinity_np(void) {
  int ret;

#if defined(__linux__)
  {
    cpu_set_t cpuset;
#if defined(__GLIBC__)
    __CPU_ZERO_S(sizeof(cpuset), &cpuset);
    __CPU_SET_S(0, sizeof(cpuset), &cpuset);
#else
    memset(&cpuset, 0, sizeof(cpuset));
#endif
    ret = pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    (void)ret;
  }
#else
  {
    unsigned char small_mask;
    unsigned char big_mask[32];
    pthread_t invalid_thread = (pthread_t)1;

    memset(big_mask, 0, sizeof(big_mask));
    big_mask[0] = 1;
    small_mask = 1;

    /* thread = 0 */
    errno = 0;
    ret = pthread_setaffinity_np((pthread_t)0, sizeof(small_mask), &small_mask);
    ASSERT_EQ(-1, ret);
    ASSERT_EQ(ESRCH, errno);

    /* NULL cpuset */
    errno = 0;
    ret = pthread_setaffinity_np(invalid_thread, sizeof(small_mask), NULL);
    ASSERT_EQ(-1, ret);
    ASSERT_EQ(EINVAL, errno);

    /* 0 cpusetsize */
    errno = 0;
    ret = pthread_setaffinity_np(invalid_thread, 0, &small_mask);
    ASSERT_EQ(-1, ret);
    ASSERT_EQ(EINVAL, errno);

    /* Zero mask (both small and oversized > sizeof(DWORD_PTR)/unsigned long) */
    small_mask = 0;
    errno = 0;
    ret =
        pthread_setaffinity_np(invalid_thread, sizeof(small_mask), &small_mask);
    ASSERT_EQ(-1, ret);
    ASSERT_EQ(EINVAL, errno);

    big_mask[0] = 0;
    errno = 0;
    ret = pthread_setaffinity_np(invalid_thread, sizeof(big_mask), big_mask);
    ASSERT_EQ(-1, ret);
    ASSERT_EQ(EINVAL, errno);

    /* Valid mask */
    small_mask = 1;
    big_mask[0] = 1;
#if defined(_MSC_VER) || defined(_WIN32)
    /* Invalid HANDLE fails SetThreadAffinityMask */
    errno = 0;
    ret =
        pthread_setaffinity_np(invalid_thread, sizeof(small_mask), &small_mask);
    ASSERT_EQ(-1, ret);
    ASSERT_EQ(EINVAL, errno);

    /* Valid GetCurrentThread() succeeds SetThreadAffinityMask */
    ret = pthread_setaffinity_np((pthread_t)GetCurrentThread(),
                                 sizeof(big_mask), big_mask);
    ASSERT_EQ(0, ret);
#else
    ret =
        pthread_setaffinity_np(invalid_thread, sizeof(small_mask), &small_mask);
    ASSERT_EQ(0, ret);
    ret = pthread_setaffinity_np(invalid_thread, sizeof(big_mask), big_mask);
    ASSERT_EQ(0, ret);
#endif
  }
#endif

  PASS();
}

SUITE(suite_bsd_pthread_np_core) {
  RUN_TEST(test_bsd_pthread_np_init);
  RUN_TEST(test_pthread_setaffinity_np);
}
