/* clang-format off */
#include "greatest.h"
/* clang-format on */

extern int sys_tree_dummy_for_coverage(void);

TEST test_sys_tree_dummy(void) { PASS(); }

SUITE(suite_bsd_sys_tree_core) { RUN_TEST(test_sys_tree_dummy); }
