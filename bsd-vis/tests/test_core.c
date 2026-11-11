/* clang-format off */
#include "greatest.h"
#include <vis.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

TEST test_bsd_vis_encode_decode(void) {
  char enc[64];
  char dec[64];
  char *alloc_enc = NULL;

  ASSERT(strvis(enc, "a\nb", VIS_CSTYLE) > 0);
  ASSERT_STR_EQ("a\\nb", enc);

  ASSERT(strunvis(dec, enc) > 0);
  ASSERT_STR_EQ("a\nb", dec);

  ASSERT(stravis(&alloc_enc, "hello\tworld", VIS_CSTYLE | VIS_TAB) > 0);
  ASSERT(alloc_enc != NULL);
  free(alloc_enc);
  PASS();
}

SUITE(suite_bsd_vis_core) { RUN_TEST(test_bsd_vis_encode_decode); }
