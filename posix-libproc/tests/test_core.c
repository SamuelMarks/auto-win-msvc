#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-libproc.h"

TEST test_libproc(void) { SKIP(); /* Stubbed */ }

SUITE(suite_posix_libproc_core) { RUN_TEST(test_libproc); }
