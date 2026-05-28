#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
/* clang-format on */

TEST test_madvise(void) { SKIP(); /* Generated stub for madvise */ }

SUITE(suite_posix_mman_madvise) { RUN_TEST(test_madvise); }
