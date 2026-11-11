/* clang-format off */
#include "greatest.h"
#include <sys/sbuf.h>
#include <string.h>
/* clang-format on */

TEST test_bsd_sbuf_ops(void) {
  struct sbuf *sb = sbuf_new_auto();
  ASSERT(sb != NULL);
  ASSERT_EQ(0, sbuf_cat(sb, "hello "));
  ASSERT_EQ(0, sbuf_printf(sb, "%d", 42));
  ASSERT_EQ(0, sbuf_finish(sb));
  ASSERT(sbuf_done(sb));
  ASSERT_STR_EQ("hello 42", sbuf_data(sb));
  ASSERT_EQ(8, (int)sbuf_len(sb));
  sbuf_delete(sb);
  PASS();
}

SUITE(suite_bsd_sbuf_core) { RUN_TEST(test_bsd_sbuf_ops); }
