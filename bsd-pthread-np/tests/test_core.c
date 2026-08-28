#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

TEST test_stub(void) { SKIP(); }

TEST test_pthread_setaffinity_np(void) {
  SKIP(); /* Generated stub for pthread_setaffinity_np */
}

SUITE(suite_bsd_pthread_np_core) {
  RUN_TEST(test_stub);
  RUN_TEST(test_pthread_setaffinity_np);
}
