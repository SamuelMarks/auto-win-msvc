#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ipc.h"
/* clang-format on */

TEST test_semget(void) {
  int semid;
  semid = semget(-1, 1, 0);
  ASSERT_EQ(-1, semid);
  PASS();
}

SUITE(suite_posix_ipc_semget) { RUN_TEST(test_semget); }
