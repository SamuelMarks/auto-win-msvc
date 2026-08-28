#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-core.h"
#include <process.h> /* For _getpid if needed */
/* clang-format on */

TEST test_getegid(void) { SKIP(); /* Generated stub for getegid */ }

TEST test_geteuid(void) { SKIP(); /* Generated stub for geteuid */ }

TEST test_getgid(void) { SKIP(); /* Generated stub for getgid */ }

TEST test_getpgid(void) { SKIP(); /* Generated stub for getpgid */ }

TEST test_getuid(void) { SKIP(); /* Generated stub for getuid */ }

TEST test_setegid(void) { SKIP(); /* Generated stub for setegid */ }

TEST test_seteuid(void) { SKIP(); /* Generated stub for seteuid */ }

TEST test_setgid(void) { SKIP(); /* Generated stub for setgid */ }

TEST test_setpgid(void) { SKIP(); /* Generated stub for setpgid */ }

TEST test_setregid(void) { SKIP(); /* Generated stub for setregid */ }

TEST test_setreuid(void) { SKIP(); /* Generated stub for setreuid */ }

TEST test_setuid(void) { SKIP(); /* Generated stub for setuid */ }

SUITE(suite_posix_core_ugid) {
  RUN_TEST(test_getegid);
  RUN_TEST(test_geteuid);
  RUN_TEST(test_getgid);
  RUN_TEST(test_getpgid);
  RUN_TEST(test_getuid);
  RUN_TEST(test_setegid);
  RUN_TEST(test_seteuid);
  RUN_TEST(test_setgid);
  RUN_TEST(test_setpgid);
  RUN_TEST(test_setregid);
  RUN_TEST(test_setreuid);
  RUN_TEST(test_setuid);
}
