#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ipc.h"
#include <string.h>
/* clang-format on */

TEST test_msgsnd(void) {
  struct {
    long mtype;
    char mtext[8];
  } msg;
  int res;
  memset(&msg, 0, sizeof(msg));
  msg.mtype = 1L;
  res = msgsnd(-1, &msg, sizeof(msg.mtext), IPC_NOWAIT);
  ASSERT_EQ(-1, res);
  PASS();
}

SUITE(suite_posix_ipc_msgsnd) { RUN_TEST(test_msgsnd); }
