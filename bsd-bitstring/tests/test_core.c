/* clang-format off */
#include "greatest.h"
#include <bitstring.h>
/* clang-format on */

TEST test_bsd_bitstring_macros(void) {
  bitstr_t *b = bit_alloc(32);
  int idx = -1;
  ASSERT(b != NULL);
  bit_set(b, 5);
  ASSERT(bit_test(b, 5));
  bit_ffs(b, 32, &idx);
  ASSERT_EQ(5, idx);
  bit_clear(b, 5);
  bit_ffs(b, 32, &idx);
  ASSERT_EQ(-1, idx);
  free(b);
  PASS();
}

SUITE(suite_bsd_bitstring_core) { RUN_TEST(test_bsd_bitstring_macros); }
