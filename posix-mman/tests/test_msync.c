#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-mman.h"

TEST test_msync(void) { SKIP(); /* Generated stub for msync */ }

SUITE(suite_posix_mman_msync) { RUN_TEST(test_msync); }
