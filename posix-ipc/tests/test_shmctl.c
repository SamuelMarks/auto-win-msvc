#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ipc.h"
/* clang-format on */

TEST test_shmctl(void) {
  int res;
  res = shmctl(-1, IPC_RMID, NULL);
  ASSERT_EQ(-1, res);
  PASS();
}

SUITE(suite_posix_ipc_shmctl) { RUN_TEST(test_shmctl); }
