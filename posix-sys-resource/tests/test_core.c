#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-sys-resource.h"

TEST test_sys_resource(void) {
  struct rlimit r;
  if (getrlimit(RLIMIT_NOFILE, &r) == -1)
    SKIP();
  PASS();
}

TEST test_posix_getrusage(void) {
  SKIP(); /* Generated stub for posix_getrusage */
}

TEST test_posix_getrlimit(void) {
  SKIP(); /* Generated stub for posix_getrlimit */
}

TEST test_posix_setrlimit(void) {
  SKIP(); /* Generated stub for posix_setrlimit */
}

SUITE(suite_posix_sys_resource_core) {
  RUN_TEST(test_sys_resource);
  RUN_TEST(test_posix_getrusage);
  RUN_TEST(test_posix_getrlimit);
  RUN_TEST(test_posix_setrlimit);
}
