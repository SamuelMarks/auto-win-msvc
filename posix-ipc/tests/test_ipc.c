#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-ipc.h"

TEST test_ipc(void) {
  key_t k = ftok(".", 1);
  if (k == (key_t)-1)
    SKIP();
  PASS();
}

SUITE(suite_posix_ipc_ipc) { RUN_TEST(test_ipc); }
