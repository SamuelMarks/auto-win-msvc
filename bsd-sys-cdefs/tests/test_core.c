/* clang-format off */
#include "greatest.h"
#include "sys/cdefs.h"
/* clang-format on */

extern int sys_cdefs_dummy_for_coverage(void);

TEST test_bsd_sys_cdefs_dummy(void) { PASS(); }

SUITE(suite_bsd_sys_cdefs_core) { RUN_TEST(test_bsd_sys_cdefs_dummy); }
