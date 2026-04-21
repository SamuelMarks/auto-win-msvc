#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"

TEST test_stub(void) { SKIP(); }

TEST test_prctl(void) { SKIP(); /* Generated stub for prctl */ }

SUITE(suite_linux_sys_prctl_core) {
  RUN_TEST(test_stub);
  RUN_TEST(test_prctl);
}
