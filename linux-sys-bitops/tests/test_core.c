#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-sys-bitops.h"
#include <stdio.h>
/* clang-format on */

TEST test_linux_sys_bitops_init(void) {
  enum linux_sys_bitops_error_code rc;
  int status;

  status = 0;
  rc = linux_sys_bitops_init(NULL);
  if (rc != LINUX_SYS_BITOPS_ERROR_NULL_POINTER) {
    printf("Expected LINUX_SYS_BITOPS_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_sys_bitops_init(&status);
  if (rc != LINUX_SYS_BITOPS_SUCCESS) {
    printf("linux_sys_bitops_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_bitops_ffs_fls(void) {
  unsigned long idx;

  ASSERT_EQ(0, posix_ffs(0));
  ASSERT_EQ(1, posix_ffs(1));
  ASSERT_EQ(2, posix_ffs(2));
  ASSERT_EQ(3, posix_ffs(4));
  ASSERT_EQ(0, posix_fls(0));
  ASSERT_EQ(1, posix_fls(1));
  ASSERT_EQ(2, posix_fls(2));
  ASSERT_EQ(3, posix_fls(4));
  ASSERT_EQ(0, posix_fls64(0));
  ASSERT_EQ(1, posix_fls64(1));

  idx = 0;
  posix___ffs(4, &idx);
  ASSERT_EQ(2, (int)idx);
  posix___ffs(4, NULL);

  idx = 0;
  posix_ffz(~4UL, &idx);
  ASSERT_EQ(2, (int)idx);
  posix_ffz(~4UL, NULL);

  PASS();
}

TEST test_bitops_manipulations(void) {
  unsigned long val;

  val = 0;
  posix_set_bit(3, &val);
  ASSERT_EQ(1, posix_test_bit(3, &val));
  ASSERT_EQ(0, posix_test_bit(2, &val));

  ASSERT_EQ(1, posix_test_and_clear_bit(3, &val));
  ASSERT_EQ(0, posix_test_bit(3, &val));

  ASSERT_EQ(0, posix_test_and_set_bit(3, &val));
  ASSERT_EQ(1, posix_test_bit(3, &val));

  ASSERT_EQ(1, posix_test_and_change_bit(3, &val));
  ASSERT_EQ(0, posix_test_bit(3, &val));

  posix_clear_bit(3, &val);
  ASSERT_EQ(0, posix_test_bit(3, &val));

  posix_change_bit(2, &val);
  ASSERT_EQ(1, posix_test_bit(2, &val));

  posix___set_bit(4, &val);
  ASSERT_EQ(1, posix_test_bit(4, &val));

  posix___clear_bit(4, &val);
  ASSERT_EQ(0, posix_test_bit(4, &val));

  posix___change_bit(4, &val);
  ASSERT_EQ(1, posix_test_bit(4, &val));

  ASSERT_EQ(1, posix___test_and_clear_bit(4, &val));
  ASSERT_EQ(0, posix___test_and_set_bit(4, &val));
  ASSERT_EQ(1, posix___test_and_change_bit(4, &val));
  ASSERT_EQ(0, posix_test_bit(4, &val));

  PASS();
}

SUITE(suite_linux_sys_bitops_core) {
  RUN_TEST(test_linux_sys_bitops_init);
  RUN_TEST(test_bitops_ffs_fls);
  RUN_TEST(test_bitops_manipulations);
}
