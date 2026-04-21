#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"

TEST test_stub(void) { SKIP(); }

TEST test_void(void) { SKIP(); /* Generated stub for void */ }

TEST test_int(void) { SKIP(); /* Generated stub for int */ }

SUITE(suite_linux_backtrace_core) {
  RUN_TEST(test_stub);
  RUN_TEST(test_void);
  RUN_TEST(test_int);
}
