#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-stropts.h"

TEST test_stropts(void) { SKIP(); }

SUITE(suite_posix_stropts_core) { RUN_TEST(test_stropts); }
