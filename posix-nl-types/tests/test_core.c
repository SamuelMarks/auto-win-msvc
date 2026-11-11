/* clang-format off */
#include "greatest.h"
#include <nl_types.h>
/* clang-format on */

extern int posix_nl_types_dummy_for_coverage(void);

TEST test_posix_nl_types_catalog(void) {
  nl_catd c = catopen("nonexistent", NL_CAT_LOCALE);
  ASSERT_STR_EQ("default msg", catgets(c, 1, 1, "default msg"));
  if (c != (nl_catd)-1 && c != (nl_catd)0) {
    catclose(c);
  }
  ASSERT_EQ(0, posix_nl_types_dummy_for_coverage());
  PASS();
}

SUITE(suite_posix_nl_types_core) { RUN_TEST(test_posix_nl_types_catalog); }
