/* clang-format off */
#include "greatest.h"
#include "sys/statfs.h"
/* clang-format on */

TEST test_sys_statfs_forwarding_header(void) {
  struct statfs sfs;

  ASSERT_EQ(0, statfs(".", &sfs));
  ASSERT_EQ(-1, statfs("/nonexistent_statfs_header_test_path_99999", &sfs));
  PASS();
}

SUITE(suite_linux_sys_statfs_header) {
  RUN_TEST(test_sys_statfs_forwarding_header);
}
