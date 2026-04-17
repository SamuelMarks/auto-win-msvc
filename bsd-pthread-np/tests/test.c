/* clang-format off */
#include "bsd-pthread-np.h"
#include "greatest.h"
#include <errno.h>

#if defined(_MSC_VER) || defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
/* clang-format on */

TEST test_pthread_setaffinity_np_basic(void) {
#if defined(_MSC_VER) || defined(_WIN32)
  unsigned char cpuset[sizeof(DWORD_PTR)];
  int res;
  size_t i;

  for (i = 0; i < sizeof(cpuset); i++) {
    cpuset[i] = 0;
  }
  cpuset[0] = 1; /* Core 0 */

  res = pthread_setaffinity_np(NULL, sizeof(cpuset), cpuset);
  ASSERT_EQ(0, res);

  res = pthread_setaffinity_np(NULL, 0, cpuset);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(EINVAL, errno);

  res = pthread_setaffinity_np(NULL, sizeof(cpuset), NULL);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(EINVAL, errno);

  cpuset[0] = 0; /* Invalid mask */
  res = pthread_setaffinity_np(NULL, sizeof(cpuset), cpuset);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(EINVAL, errno);
#endif
  PASS();
}

SUITE(suite) { RUN_TEST(test_pthread_setaffinity_np_basic); }

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite);
  GREATEST_MAIN_END();
}
