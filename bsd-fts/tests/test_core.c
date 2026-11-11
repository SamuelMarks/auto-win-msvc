/* clang-format off */
#include "greatest.h"
#include <fts.h>
/* clang-format on */

TEST test_bsd_fts_walk(void) {
  char *paths[2];
  FTS *ftsp;
  FTSENT *p;
  int count = 0;

  paths[0] = ".";
  paths[1] = NULL;
  ftsp = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
  ASSERT(ftsp != NULL);
  while ((p = fts_read(ftsp)) != NULL) {
    count++;
    if (p->fts_level >= 1) {
      fts_set(ftsp, p, FTS_SKIP);
    }
  }
  ASSERT(count > 0);
  ASSERT_EQ(0, fts_close(ftsp));
  PASS();
}

SUITE(suite_bsd_fts_core) { RUN_TEST(test_bsd_fts_walk); }
