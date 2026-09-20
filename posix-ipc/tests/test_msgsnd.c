#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ipc.h"
#include <string.h>
/* clang-format on */

TEST test_msgsnd(void) {
  char buf[16];
  int res;
  memset(buf, 0, sizeof(buf));
  res = msgsnd(-1, buf, sizeof(buf), IPC_NOWAIT);
  ASSERT_EQ(-1, res);
  PASS();
}

SUITE(suite_posix_ipc_msgsnd) { RUN_TEST(test_msgsnd); }
