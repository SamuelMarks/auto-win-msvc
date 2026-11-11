/* clang-format off */
#include "greatest.h"
#include <stringlist.h>
#include <string.h>
/* clang-format on */

TEST test_bsd_stringlist_ops(void) {
  StringList *sl = sl_init();
  ASSERT(sl != NULL);
  ASSERT_EQ(0, sl_add(sl, "one"));
  ASSERT_EQ(0, sl_add(sl, "two"));
  ASSERT_STR_EQ("one", sl_find(sl, "one"));
  ASSERT_EQ(0, sl_delete(sl, "one", 0));
  ASSERT(sl_find(sl, "one") == NULL);
  sl_free(sl, 0);
  PASS();
}

SUITE(suite_bsd_stringlist_core) { RUN_TEST(test_bsd_stringlist_ops); }
