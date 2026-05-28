#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

TEST test_stub(void) { SKIP(); }

TEST test_je_malloc_stats_print(void) {
  SKIP(); /* Generated stub for je_malloc_stats_print */
}

SUITE(suite_bsd_malloc_np_core) {
  RUN_TEST(test_stub);
  RUN_TEST(test_je_malloc_stats_print);
}
