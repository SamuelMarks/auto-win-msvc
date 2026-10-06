#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-sys-bitops.h"
#include <stdio.h>
/* clang-format on */

TEST test_linux_sys_bitops_init(void) {
  auto_win_msvc_error_t rc;
  int status = 0;
  (void)rc;
  (void)status;
  rc = linux_sys_bitops_init(NULL);

  rc = linux_sys_bitops_init(&status);

  PASS();
}

TEST test_bitops_basic(void) {
  unsigned long out_idx;
  (void)out_idx;

  /* ffs */
  ASSERT_EQ(0, posix_ffs(0));
  ASSERT_EQ(1, posix_ffs(1));
  ASSERT_EQ(2, posix_ffs(2));
  ASSERT_EQ(5, posix_ffs(16));

  /* fls */
  ASSERT_EQ(0, posix_fls(0));
  ASSERT_EQ(1, posix_fls(1));
  ASSERT_EQ(2, posix_fls(2));
  ASSERT_EQ(5, posix_fls(16));
  ASSERT_EQ(32, posix_fls(0x80000000));

  /* __ffs */
  posix___ffs(0x10, &out_idx);
  ASSERT_EQ(4, out_idx);

  /* ffz */
  posix_ffz(~(0x10UL), &out_idx);
  ASSERT_EQ(4, out_idx);

  /* Provide a zero word for posix___ffs coverage */
#if defined(_MSC_VER)
  /* MSVC intrinsics undefined for 0, but our wrapper might allow it */
#endif

  /* Check fls64 */
  ASSERT_EQ(0, posix_fls64(0));
  ASSERT_EQ(1, posix_fls64(1));
#if defined(_MSC_VER)
  {
    unsigned __int64 val1 = 1;
    unsigned __int64 val2 = 1;
    val1 <<= 32;
    val2 <<= 63;
    ASSERT_EQ(33, posix_fls64(val1));
    ASSERT_EQ(64, posix_fls64(val2));
  }
#else
  {
    __extension__ unsigned long long val1 = 1;
    __extension__ unsigned long long val2 = 1;
    val1 <<= 32;
    val2 <<= 63;
    ASSERT_EQ(33, posix_fls64(val1));
    ASSERT_EQ(64, posix_fls64(val2));
  }
#endif

  PASS();
}

TEST test_bitops_modifications(void) {
  unsigned long bits = 0;
  int res;

  /* Atomic-like operations */
  (void)posix_set_bit(0, &bits);
  ASSERT_EQ(1, bits);

  /* Test again when set to hit "old != 0" branches */
  (void)posix_set_bit(0, &bits);
  ASSERT_EQ(1, bits);
  (void)posix_clear_bit(0, &bits);
  ASSERT_EQ(0, bits);
  (void)posix_clear_bit(0, &bits);
  ASSERT_EQ(0, bits);

  (void)posix_change_bit(0, &bits);
  ASSERT_EQ(1, bits);

  (void)posix_change_bit(0, &bits);
  ASSERT_EQ(0, bits);

  /* test and set / clear / change */
  bits = 0;
  res = posix_test_and_set_bit(1, &bits);
  ASSERT_EQ(0, res);
  ASSERT_EQ(2, bits);
  res = posix_test_and_set_bit(1, &bits);
  ASSERT(res != 0);

  res = posix_test_and_clear_bit(1, &bits);
  ASSERT(res != 0);
  ASSERT_EQ(0, bits);
  res = posix_test_and_clear_bit(1, &bits);
  ASSERT_EQ(0, res);

  res = posix_test_and_change_bit(2, &bits);
  ASSERT_EQ(0, res);
  ASSERT_EQ(4, bits);
  res = posix_test_and_change_bit(2, &bits);
  ASSERT(res != 0);
  ASSERT_EQ(0, bits);

  /* Underscore versions */
  posix___set_bit(3, &bits);
  ASSERT_EQ(8, bits);
  posix___clear_bit(3, &bits);
  ASSERT_EQ(0, bits);
  posix___change_bit(4, &bits);
  ASSERT_EQ(16, bits);

  res = posix___test_and_set_bit(5, &bits);
  ASSERT_EQ(0, res);
  res = posix___test_and_set_bit(5, &bits);
  ASSERT(res != 0);

  res = posix___test_and_clear_bit(5, &bits);
  ASSERT(res != 0);
  res = posix___test_and_clear_bit(5, &bits);
  ASSERT_EQ(0, res);

  res = posix___test_and_change_bit(6, &bits);
  ASSERT_EQ(0, res);
  res = posix___test_and_change_bit(6, &bits);
  ASSERT(res != 0);

  res = posix_test_bit(4, &bits);
  ASSERT(res != 0);
  res = posix_test_bit(5, &bits);
  ASSERT_EQ(0, res);

  PASS();
}

SUITE(suite_linux_sys_bitops_core) {
  RUN_TEST(test_linux_sys_bitops_init);
  RUN_TEST(test_bitops_basic);
  RUN_TEST(test_bitops_modifications);
}

TEST test_posix___ffs_null_out_index(void) {
  error_type_t err;
  (void)err;
  err = posix___ffs(0x10, NULL);

  PASS();
}

SUITE(suite_linux_sys_bitops_extra) {
  RUN_TEST(test_posix___ffs_null_out_index);
}
