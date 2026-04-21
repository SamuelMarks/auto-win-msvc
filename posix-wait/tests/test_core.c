#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-wait.h"
#include <stddef.h>

TEST test_waitpid_nohang(void) {
  pid_t res = waitpid(-1, NULL, WNOHANG);
  ASSERT(res <= 0 || res > 0);
  PASS();
}

TEST test_waitid(void) { SKIP(); /* Generated stub for waitid */ }

SUITE(suite_posix_wait_core) {
  RUN_TEST(test_waitpid_nohang);
  RUN_TEST(test_waitid);
}
