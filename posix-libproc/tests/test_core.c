#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-libproc.h"
/* clang-format on */

TEST test_libproc(void) { SKIP(); /* Stubbed */ }

SUITE(suite_posix_libproc_core) { RUN_TEST(test_libproc); }
