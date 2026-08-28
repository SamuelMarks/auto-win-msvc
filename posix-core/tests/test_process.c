#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-core.h"
#include <process.h> /* For _getpid if needed */
/* clang-format on */

TEST test_core_getpid(void) {
  /* pid_t pid = getpid(); */
  PASS();
}

TEST test_fexecve(void) { SKIP(); /* Generated stub for fexecve */ }

TEST test_fork(void) { SKIP(); /* Generated stub for fork */ }

TEST test_getppid(void) { SKIP(); /* Generated stub for getppid */ }

TEST test_vfork(void) { SKIP(); /* Generated stub for vfork */ }

SUITE(suite_posix_core_process) {
  RUN_TEST(test_core_getpid);
  RUN_TEST(test_fexecve);
  RUN_TEST(test_fork);
  RUN_TEST(test_getppid);
  RUN_TEST(test_vfork);
}
