#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ipc.h"
/* clang-format on */

TEST test_semop(void) {
  struct sembuf sop;
  int res;
  sop.sem_num = 0;
  sop.sem_op = 0;
  sop.sem_flg = 0;
  res = semop(-1, &sop, 1);
  ASSERT_EQ(-1, res);
  PASS();
}

SUITE(suite_posix_ipc_semop) { RUN_TEST(test_semop); }
