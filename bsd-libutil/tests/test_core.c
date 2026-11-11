/* clang-format off */
#include "greatest.h"
#include <libutil.h>
#include <login_cap.h>
#include <string.h>
/* clang-format on */

TEST test_bsd_libutil_humanize_expand(void) {
  char buf[32];
  uint64_t val = 0;
  login_cap_t *lc;
  char host[32] = "myhost.example.com";

  ASSERT(humanize_number(buf, sizeof(buf), 1024, "B", HN_AUTOSCALE,
                         HN_NOSPACE) > 0);
  ASSERT_STR_EQ("1KB", buf);

  ASSERT_EQ(0, expand_number("16K", &val));
  ASSERT_EQ((uint64_t)16384UL, val);

  trimdomain(host, sizeof(host));
  ASSERT_STR_EQ("myhost", host);

  lc = login_getclass(NULL);
  ASSERT(lc != NULL);
  ASSERT_EQ(1, login_getcapbool(lc, "test", 1));
  ASSERT_EQ(42, login_getcapnum(lc, "num", 42, -1));
  login_close(lc);
  PASS();
}

SUITE(suite_bsd_libutil_core) { RUN_TEST(test_bsd_libutil_humanize_expand); }
