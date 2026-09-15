#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ipc.h"
/* clang-format on */

TEST test_msgrcv(void) {
  char buf[16];
  ssize_t ssz;
  ssz = msgrcv(-1, buf, sizeof(buf), 0, IPC_NOWAIT);
  ASSERT_EQ(-1, ssz);
  PASS();
}

SUITE(suite_posix_ipc_msgrcv) { RUN_TEST(test_msgrcv); }
