#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "bsd-pthread-np.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_bsd_pthread_np_init(void) {
  enum bsd_pthread_np_error_code rc;
  int status;

  status = 0;
  rc = bsd_pthread_np_init(NULL);
  if (rc != BSD_PTHREAD_NP_ERROR_NULL_POINTER) {
    printf("Expected BSD_PTHREAD_NP_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = bsd_pthread_np_init(&status);
  if (rc != BSD_PTHREAD_NP_SUCCESS) {
    printf("bsd_pthread_np_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_pthread_setaffinity_np(void) {
  unsigned long mask;
  int ret;

  /* NULL cpuset */
  mask = 1;
  ret = pthread_setaffinity_np(0, sizeof(mask), NULL);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EINVAL, errno);

  /* 0 cpusetsize */
  ret = pthread_setaffinity_np(0, 0, &mask);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EINVAL, errno);

  /* Zero mask */
  mask = 0;
  ret = pthread_setaffinity_np(0, sizeof(mask), &mask);
  ASSERT_EQ(-1, ret);
  ASSERT_EQ(EINVAL, errno);

  /* Valid mask, thread = 0 */
  mask = 1;
  ret = pthread_setaffinity_np(0, sizeof(mask), &mask);
  ASSERT_EQ(0, ret);

  PASS();
}

SUITE(suite_bsd_pthread_np_core) {
  RUN_TEST(test_bsd_pthread_np_init);
  RUN_TEST(test_pthread_setaffinity_np);
}
