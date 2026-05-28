#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

TEST test_stub(void) { SKIP(); }

TEST test_backtrace(void) { SKIP(); /* Generated stub for backtrace */ }

TEST test_backtrace_symbols(void) {
  SKIP(); /* Generated stub for backtrace_symbols */
}

TEST test_backtrace_symbols_fd(void) {
  SKIP(); /* Generated stub for backtrace_symbols_fd */
}

SUITE(suite_linux_execinfo_core) {
  RUN_TEST(test_stub);
  RUN_TEST(test_backtrace);
  RUN_TEST(test_backtrace_symbols);
  RUN_TEST(test_backtrace_symbols_fd);
}
