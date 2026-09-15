#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ipc.h"
/* clang-format on */

TEST test_msgget(void) {
  int msqid;
  msqid = msgget(-1, 0);
  ASSERT_EQ(-1, msqid);
  PASS();
}

SUITE(suite_posix_ipc_msgget) { RUN_TEST(test_msgget); }
