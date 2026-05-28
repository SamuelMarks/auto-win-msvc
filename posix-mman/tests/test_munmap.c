#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
/* clang-format on */

TEST test_munmap(void) { SKIP(); /* Generated stub for munmap */ }

SUITE(suite_posix_mman_munmap) { RUN_TEST(test_munmap); }
