#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-times.h"

TEST test_times(void) { SKIP(); }

SUITE(suite_posix_times_core) { RUN_TEST(test_times); }
