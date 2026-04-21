#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-mman.h"

TEST test_munlockall(void) { SKIP(); /* Generated stub for munlockall */ }

SUITE(suite_posix_mman_munlockall) { RUN_TEST(test_munlockall); }
