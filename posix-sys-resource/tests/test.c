/* clang-format off */
#include "greatest.h"
#include "posix-sys-resource.h"
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)

TEST test_sys_resource(void) {
  struct rusage usage;
  struct rlimit rlp;

  ASSERT_EQ_FMT(0, posix_getrusage(RUSAGE_SELF, &usage), "%d");
  /* Memory shouldn't be exactly 0 */
  ASSERT(usage.ru_maxrss > 0);

  ASSERT_EQ_FMT(0, posix_getrlimit(RLIMIT_NOFILE, &rlp), "%d");
  ASSERT(rlp.rlim_cur >= 512);

  rlp.rlim_cur = 2048;
  ASSERT_EQ_FMT(0, posix_setrlimit(RLIMIT_NOFILE, &rlp), "%d");

  posix_getrlimit(RLIMIT_NOFILE, &rlp);
  ASSERT_EQ_FMT(2048, (int)rlp.rlim_cur, "%d");

  PASS();
}

SUITE(sys_resource_suite) { RUN_TEST(test_sys_resource); }

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(sys_resource_suite);
  GREATEST_MAIN_END();
}
#else
int main(void) { return 0; }
#endif
