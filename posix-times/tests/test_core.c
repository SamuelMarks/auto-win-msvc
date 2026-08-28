#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-times.h"
/* clang-format on */

TEST test_times(void) { SKIP(); }

SUITE(suite_posix_times_core) { RUN_TEST(test_times); }
