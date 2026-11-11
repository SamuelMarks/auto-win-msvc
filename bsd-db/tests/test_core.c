/* clang-format off */
#include "greatest.h"
#include <db.h>
#include <string.h>
/* clang-format on */

TEST test_bsd_db_inmemory(void) {
  DB *db = dbopen(NULL, 0, 0600, DB_BTREE, NULL);
  DBT k, v, out;
  ASSERT(db != NULL);
  k.data = "key1";
  k.size = 5;
  v.data = "val1";
  v.size = 5;
  ASSERT_EQ(RET_SUCCESS, db->put(db, &k, &v, 0));
  ASSERT_EQ(RET_SUCCESS, db->get(db, &k, &out, 0));
  ASSERT_STR_EQ("val1", (const char *)out.data);
  ASSERT_EQ(RET_SUCCESS, db->seq(db, &k, &out, R_FIRST));
  ASSERT_EQ(RET_SUCCESS, db->del(db, &k, R_CURSOR));
  ASSERT_EQ(RET_SUCCESS, db->close(db));
  PASS();
}

SUITE(suite_bsd_db_core) { RUN_TEST(test_bsd_db_inmemory); }
