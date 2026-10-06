#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "bsd-pthread-np.h"
#include <errno.h>
#include <stdio.h>
#if defined(__linux__)
#include <sched.h>
#endif
/* clang-format on */

TEST test_bsd_pthread_np_init(void) {
  auto_win_msvc_error_t rc;
  int status;

  status = 0;
  rc = bsd_pthread_np_init(NULL);
  (void)rc;

  rc = bsd_pthread_np_init(&status);
  (void)rc;

  (void)status;
  PASS();
}

TEST test_pthread_setaffinity_np(void) {
  int ret;

#if defined(__linux__)
  {
    cpu_set_t cpuset;
    __CPU_ZERO_S(sizeof(cpuset), &cpuset);
    __CPU_SET_S(0, sizeof(cpuset), &cpuset);
    ret = pthread_setaffinity_np((pthread_t)0, sizeof(cpuset), &cpuset);
    (void)ret;
  }
#else
  {
    unsigned long mask;
    pthread_t dummy_thread = (pthread_t)1;

    mask = 1;
    /* thread = 0 */
    ret = pthread_setaffinity_np((pthread_t)0, sizeof(mask), &mask);
    (void)ret;
    (void)errno;

    /* NULL cpuset */
    ret = pthread_setaffinity_np(dummy_thread, sizeof(mask), NULL);
    (void)ret;
    (void)errno;

    /* 0 cpusetsize */
    ret = pthread_setaffinity_np(dummy_thread, 0, &mask);
    (void)ret;
    (void)errno;

    /* Zero mask */
    mask = 0;
    ret = pthread_setaffinity_np(dummy_thread, sizeof(mask), &mask);
    (void)ret;
    (void)errno;

    /* Valid mask */
    mask = 1;
#if defined(_MSC_VER) || defined(_WIN32)
    /* on Windows, dummy_thread=1 might fail SetThreadAffinityMask */
    ret = pthread_setaffinity_np(dummy_thread, sizeof(mask), &mask);
    (void)ret;
    (void)errno;
#else
    /* Fallback returns 0 */
    ret = pthread_setaffinity_np(dummy_thread, sizeof(mask), &mask);
    (void)ret;
#endif
  }
#endif

  PASS();
}

SUITE(suite_bsd_pthread_np_core) {
  RUN_TEST(test_bsd_pthread_np_init);
  RUN_TEST(test_pthread_setaffinity_np);
}
