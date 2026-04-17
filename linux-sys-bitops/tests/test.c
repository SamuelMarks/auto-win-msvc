/* clang-format off */
#include "greatest.h"
#include "linux-sys-bitops.h"
/* clang-format on */

TEST test_ffs(void) {
  ASSERT_EQ(0, ffs(0));
  ASSERT_EQ(1, ffs(1));
  ASSERT_EQ(2, ffs(2));
  ASSERT_EQ(3, ffs(4));
  ASSERT_EQ(32, ffs((int)0x80000000));
  PASS();
}

TEST test_fls(void) {
  ASSERT_EQ(0, fls(0));
  ASSERT_EQ(1, fls(1));
  ASSERT_EQ(2, fls(2));
  ASSERT_EQ(3, fls(4));
  ASSERT_EQ(32, fls((int)0x80000000));
  PASS();
}

#if defined(_MSC_VER)
#define U64_CONST(x) ((unsigned __int64)x)
#else
__extension__ typedef unsigned long long posix_u64;
#define U64_CONST(x) ((posix_u64)x)
#endif

TEST test_fls64(void) {
  ASSERT_EQ(0, fls64(0));
  ASSERT_EQ(1, fls64(1));
  ASSERT_EQ(32, fls64(U64_CONST(0x80000000)));
  ASSERT_EQ(33, fls64(U64_CONST(1) << 32));
  ASSERT_EQ(64, fls64(U64_CONST(1) << 63));
  PASS();
}

TEST test___ffs(void) {
  ASSERT_EQ(0, __ffs(1));
  ASSERT_EQ(1, __ffs(2));
  ASSERT_EQ(31, __ffs((unsigned long)0x80000000));
  PASS();
}

TEST test_ffz(void) {
  ASSERT_EQ(0, ffz(0));
  ASSERT_EQ(0, ffz(2));
  ASSERT_EQ(1, ffz(1));
  ASSERT_EQ(31, ffz(~((unsigned long)0x80000000)));
  PASS();
}

TEST test_bitops(void) {
  unsigned long bitmap[2] = {0, 0};
  int bits_per_long = (int)(sizeof(unsigned long) * 8);

  set_bit(0, bitmap);
  ASSERT_EQ(1UL, bitmap[0]);
  ASSERT_EQ(0UL, bitmap[1]);

  set_bit(bits_per_long, bitmap);
  ASSERT_EQ(1UL, bitmap[0]);
  ASSERT_EQ(1UL, bitmap[1]);

  clear_bit(0, bitmap);
  ASSERT_EQ(0UL, bitmap[0]);
  ASSERT_EQ(1UL, bitmap[1]);

  change_bit(bits_per_long, bitmap);
  ASSERT_EQ(0UL, bitmap[0]);
  ASSERT_EQ(0UL, bitmap[1]);

  ASSERT_EQ(0, test_and_set_bit(5, bitmap));
  ASSERT_EQ(1, test_bit(5, bitmap));

  ASSERT_EQ(1, test_and_clear_bit(5, bitmap));
  ASSERT_EQ(0, test_bit(5, bitmap));

  ASSERT_EQ(0, test_and_change_bit(5, bitmap));
  ASSERT_EQ(1, test_bit(5, bitmap));

  /* Non-atomic ops */
  __set_bit(1, bitmap);
  ASSERT_EQ(1, test_bit(1, bitmap));
  __clear_bit(1, bitmap);
  ASSERT_EQ(0, test_bit(1, bitmap));
  __change_bit(1, bitmap);
  ASSERT_EQ(1, test_bit(1, bitmap));
  ASSERT_EQ(1, __test_and_clear_bit(1, bitmap));
  ASSERT_EQ(0, __test_and_set_bit(1, bitmap));
  ASSERT_EQ(1, __test_and_change_bit(1, bitmap));

  PASS();
}

SUITE(bitops_suite) {
  RUN_TEST(test_ffs);
  RUN_TEST(test_fls);
  RUN_TEST(test_fls64);
  RUN_TEST(test___ffs);
  RUN_TEST(test_ffz);
  RUN_TEST(test_bitops);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  (void)argc;
  (void)argv;
  RUN_SUITE(bitops_suite);
  GREATEST_MAIN_END();
}
