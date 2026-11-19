/* clang-format off */
#include "greatest.h"
#include <stdlib.h>

static int g_mock_bitstring_oom = 0;

static void *mock_bitstring_calloc(size_t nmemb, size_t size) {
  if (g_mock_bitstring_oom) {
    return NULL;
  }
  return calloc(nmemb, size);
}

#define BSD_BITSTRING_CALLOC mock_bitstring_calloc
#include <bitstring.h>
/* clang-format on */

TEST test_bsd_bitstring_macros(void) {
  bitstr_t *b;
  bitstr_t bit_decl(stack_bits, 16);
  int idx = -1;

  ASSERT_EQ(0, bsd_bitstring_dummy_for_coverage());

  /* OOM path for bit_alloc */
  g_mock_bitstring_oom = 1;
  b = bit_alloc(32);
  ASSERT(b == NULL);
  g_mock_bitstring_oom = 0;

  b = bit_alloc(16);
  ASSERT(b != NULL);

  memset(stack_bits, 0, sizeof(stack_bits));
  bit_set(stack_bits, 3);
  ASSERT(bit_test(stack_bits, 3) != 0);

  /* Single set/clear + bit_ffs */
  bit_ffs(b, 16, &idx);
  ASSERT_EQ(-1, idx);

  bit_set(b, 5);
  ASSERT(bit_test(b, 5) != 0);
  bit_ffs(b, 16, &idx);
  ASSERT_EQ(5, idx);

  bit_clear(b, 5);
  ASSERT_EQ(0, bit_test(b, 5));
  bit_ffs(b, 16, &idx);
  ASSERT_EQ(-1, idx);

  /* bit_nset & bit_nclear: both normal (start <= stop) and empty (start > stop)
   */
  bit_nset(b, 10, 5);
  bit_ffs(b, 16, &idx);
  ASSERT_EQ(-1, idx);

  bit_nset(b, 0, 15);
  bit_ffs(b, 16, &idx);
  ASSERT_EQ(0, idx);

  /* bit_ffc when all bits are set -> -1 */
  bit_ffc(b, 16, &idx);
  ASSERT_EQ(-1, idx);

  /* bit_nclear empty range (start > stop) leaves all set */
  bit_nclear(b, 8, 4);
  bit_ffc(b, 16, &idx);
  ASSERT_EQ(-1, idx);

  /* bit_nclear normal range clears bits 4..7 */
  bit_nclear(b, 4, 7);
  bit_ffc(b, 16, &idx);
  ASSERT_EQ(4, idx);

  free(b);
  PASS();
}

SUITE(suite_bsd_bitstring_core) { RUN_TEST(test_bsd_bitstring_macros); }
