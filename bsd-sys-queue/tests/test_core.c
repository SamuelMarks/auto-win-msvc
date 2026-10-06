#include "greatest.h"

extern int sys_queue_dummy_for_coverage(void);

TEST test_sys_queue_dummy(void) { PASS(); }

SUITE(suite_bsd_sys_queue_core) { RUN_TEST(test_sys_queue_dummy); }
