#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"

TEST test_stub(void) { SKIP(); }

TEST test_pthread_setaffinity_np(void) {
  SKIP(); /* Generated stub for pthread_setaffinity_np */
}

SUITE(suite_bsd_pthread_np_core) {
  RUN_TEST(test_stub);
  RUN_TEST(test_pthread_setaffinity_np);
}
