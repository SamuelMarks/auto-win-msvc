#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-stdatomic.h"

TEST test_stdatomic(void) {
  atomic_int a;
  atomic_init(&a, 0);
  PASS();
}

SUITE(suite_posix_stdatomic_core) { RUN_TEST(test_stdatomic); }
