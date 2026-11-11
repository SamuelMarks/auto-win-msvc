/* clang-format off */
#include "greatest.h"
#include <utmpx.h>
#include <timeconv.h>
#include <ttyent.h>
/* clang-format on */

TEST test_bsd_utmpx_and_ttyent(void) {
  struct utmpx *u;
  struct ttyent *t;

  setutxent();
  u = getutxent();
  ASSERT(u != NULL);
  ASSERT_EQ(USER_PROCESS, u->ut_type);
  endutxent();

  ASSERT_EQ(100, _time_to_time32(_time32_to_time(100)));

  setttyent();
  t = getttyent();
  ASSERT(t != NULL);
  endttyent();
  PASS();
}

SUITE(suite_bsd_utmpx_core) { RUN_TEST(test_bsd_utmpx_and_ttyent); }
