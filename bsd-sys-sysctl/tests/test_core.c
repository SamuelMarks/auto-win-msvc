#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"

TEST test_stub(void) { SKIP(); }

TEST test_sysctl(void) { SKIP(); /* Generated stub for sysctl */ }

TEST test_sysctlbyname(void) { SKIP(); /* Generated stub for sysctlbyname */ }

SUITE(suite_bsd_sys_sysctl_core) {
  RUN_TEST(test_stub);
  RUN_TEST(test_sysctl);
  RUN_TEST(test_sysctlbyname);
}
