#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ipc.h"
/* clang-format on */

TEST test_shmget(void) {
  int shmid;
  shmid = shmget(-1, 1024, 0);
  ASSERT_EQ(-1, shmid);
  PASS();
}

SUITE(suite_posix_ipc_shmget) { RUN_TEST(test_shmget); }
