/* clang-format off */
#include "greatest.h"
#include <db.h>
#include <ndbm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

extern void *(*g_bsd_db_calloc)(size_t, size_t);
extern void *(*g_bsd_db_malloc)(size_t);
extern void *(*g_bsd_db_realloc)(void *, size_t);
extern void bsd_db_reset_alloc_hooks(void);

static int g_db_calloc_fail_at = -1;
static int g_db_malloc_fail_at = -1;
static int g_db_realloc_fail = 0;

static void *mock_db_calloc(size_t nmemb, size_t size) {
  if (g_db_calloc_fail_at == 0) {
    return NULL;
  }
  if (g_db_calloc_fail_at > 0) {
    g_db_calloc_fail_at--;
  }
  return calloc(nmemb, size);
}

static void *mock_db_malloc(size_t size) {
  if (g_db_malloc_fail_at == 0) {
    return NULL;
  }
  if (g_db_malloc_fail_at > 0) {
    g_db_malloc_fail_at--;
  }
  return malloc(size);
}

static void *mock_db_realloc(void *ptr, size_t size) {
  if (g_db_realloc_fail) {
    return NULL;
  }
  return realloc(ptr, size);
}

TEST test_bsd_db_methods_and_cursor(void) {
  DB *db = dbopen(NULL, 0, 0600, DB_BTREE, NULL);
  DB *raw_db;
  int (*fn_close)(DB *);
  DBT k;
  DBT v;
  DBT outk;
  DBT outv;
  char kbuf[16];
  int i;

  ASSERT(db != NULL);
  fn_close = db->close;
  memset(&k, 0, sizeof(k));
  memset(&v, 0, sizeof(v));
  memset(&outk, 0, sizeof(outk));
  memset(&outv, 0, sizeof(outv));

  /* Method NULL argument checks + sync/fd + empty seq/del */
  ASSERT_EQ(RET_ERROR, fn_close(NULL));
  ASSERT_EQ(RET_ERROR, db->get(NULL, &k, &outv, 0));
  ASSERT_EQ(RET_ERROR, db->get(db, NULL, &outv, 0));
  ASSERT_EQ(RET_ERROR, db->get(db, &k, NULL, 0));

  ASSERT_EQ(RET_ERROR, db->put(NULL, &k, &v, 0));
  ASSERT_EQ(RET_ERROR, db->put(db, NULL, &v, 0));
  ASSERT_EQ(RET_ERROR, db->put(db, &k, NULL, 0));

  ASSERT_EQ(RET_ERROR, db->del(NULL, &k, 0));
  ASSERT_EQ(RET_ERROR, db->del(db, NULL, 0));
  ASSERT_EQ(RET_SPECIAL, db->del(db, NULL, R_CURSOR));

  ASSERT_EQ(RET_ERROR, db->seq(NULL, &outk, &outv, R_FIRST));
  ASSERT_EQ(RET_ERROR, db->seq(db, NULL, &outv, R_FIRST));
  ASSERT_EQ(RET_ERROR, db->seq(db, &outk, NULL, R_FIRST));
  ASSERT_EQ(RET_SPECIAL, db->seq(db, &outk, &outv, R_FIRST));

  ASSERT_EQ(RET_SUCCESS, db->sync(db, 0));
  ASSERT_EQ(-1, db->fd(db));

  /* Close DB when internal == NULL */
  raw_db = (DB *)calloc(1, sizeof(DB));
  ASSERT(raw_db != NULL);
  raw_db->internal = NULL;
  ASSERT_EQ(RET_SUCCESS, fn_close(raw_db));

  /* Zero-length key and zero-length value + overwrite with 0-length & non-zero
   */
  k.data = "";
  k.size = 0;
  v.data = "";
  v.size = 0;
  ASSERT_EQ(RET_SUCCESS, db->put(db, &k, &v, 0));
  ASSERT_EQ(RET_SPECIAL, db->put(db, &k, &v, R_NOOVERWRITE));
  ASSERT_EQ(RET_SUCCESS, db->put(db, &k, &v, 0));
  v.data = "nonempty";
  v.size = 9;
  ASSERT_EQ(RET_SUCCESS, db->put(db, &k, &v, 0));
  ASSERT_EQ(RET_SUCCESS, db->get(db, &k, &outv, 0));
  ASSERT_STR_EQ("nonempty", (const char *)outv.data);

  /* Prefix key comparisons (shorter vs longer, longer vs shorter, mismatch) */
  k.data = "ab";
  k.size = 2;
  v.data = "v_ab";
  v.size = 5;
  ASSERT_EQ(RET_SUCCESS, db->put(db, &k, &v, 0));

  k.data = "abc";
  k.size = 3;
  v.data = "v_abc";
  v.size = 6;
  ASSERT_EQ(RET_SUCCESS, db->put(db, &k, &v, 0));

  k.data = "a";
  k.size = 1;
  ASSERT_EQ(RET_SPECIAL, db->get(db, &k, &outv, 0));
  k.data = "abcd";
  k.size = 4;
  ASSERT_EQ(RET_SPECIAL, db->get(db, &k, &outv, 0));
  k.data = "ax";
  k.size = 2;
  ASSERT_EQ(RET_SPECIAL, db->get(db, &k, &outv, 0));

  /* Grow capacity past initial 16 -> 32 */
  for (i = 0; i < 20; i++) {
#if defined(_MSC_VER)
    sprintf_s(kbuf, sizeof(kbuf), "k_%02d", i);
#else
    sprintf(kbuf, "k_%02d", i);
#endif
    k.data = kbuf;
    k.size = strlen(kbuf) + 1;
    v.data = kbuf;
    v.size = k.size;
    ASSERT_EQ(RET_SUCCESS, db->put(db, &k, &v, 0));
  }

  /* seq when !started with R_NEXT -> starts at 0 */
  ASSERT_EQ(RET_SUCCESS, db->seq(db, &outk, &outv, R_NEXT));
  /* R_PREV at index 0 -> RET_SPECIAL */
  ASSERT_EQ(RET_SPECIAL, db->seq(db, &outk, &outv, R_PREV));
  /* R_CURSOR when started -> returns current item */
  ASSERT_EQ(RET_SUCCESS, db->seq(db, &outk, &outv, R_CURSOR));
  /* R_NEXT advances -> R_PREV steps back */
  ASSERT_EQ(RET_SUCCESS, db->seq(db, &outk, &outv, R_NEXT));
  ASSERT_EQ(RET_SUCCESS, db->seq(db, &outk, &outv, R_PREV));
  /* del(R_CURSOR) at seq_idx == 0 shifts remaining items and resets started=0
   */
  ASSERT_EQ(RET_SUCCESS, db->del(db, NULL, R_CURSOR));

  /* R_FIRST, R_LAST, R_NEXT at end -> RET_SPECIAL */
  ASSERT_EQ(RET_SUCCESS, db->seq(db, &outk, &outv, R_FIRST));
  ASSERT_EQ(RET_SUCCESS, db->seq(db, &outk, &outv, R_LAST));
  ASSERT_EQ(RET_SPECIAL, db->seq(db, &outk, &outv, R_NEXT));

  /* Delete last item by key so count shrinks to seq_idx, then del(R_CURSOR)
   * hits seq_idx >= count -> RET_SPECIAL */
  k.data = "k_19";
  k.size = 5;
  ASSERT_EQ(RET_SUCCESS, db->del(db, &k, 0));
  ASSERT_EQ(RET_SPECIAL, db->del(db, NULL, R_CURSOR));

  /* Reposition to R_LAST (seq_idx > 0) and del(R_CURSOR) -> decrements seq_idx
   */
  ASSERT_EQ(RET_SUCCESS, db->seq(db, &outk, &outv, R_LAST));
  ASSERT_EQ(RET_SUCCESS, db->del(db, NULL, R_CURSOR));

  /* Delete middle item by key (shifts elements) + missing key */
  k.data = "ab";
  k.size = 2;
  ASSERT_EQ(RET_SUCCESS, db->del(db, &k, 0));
  ASSERT_EQ(RET_SPECIAL, db->del(db, &k, 0));

  ASSERT_EQ(RET_SUCCESS, db->close(db));
  PASS();
}

TEST test_bsd_ndbm_and_oom(void) {
  DBM *dbm;
  DB *db;
  char k1_str[] = "alpha";
  char k2_str[] = "beta";
  char v1_str[] = "one";
  char v2_str[] = "two";
  char miss_str[] = "missing";
  datum k1;
  datum k2;
  datum v1;
  datum v2;
  datum miss;
  datum got;
  DBT dk;
  DBT dv;

  k1.dptr = k1_str;
  k1.dsize = 6;
  k2.dptr = k2_str;
  k2.dsize = 5;
  v1.dptr = v1_str;
  v1.dsize = 4;
  v2.dptr = v2_str;
  v2.dsize = 4;
  miss.dptr = miss_str;
  miss.dsize = 8;

  /* NULL safety across all ndbm functions */
  dbm_close(NULL);
  got = dbm_fetch(NULL, k1);
  ASSERT(got.dptr == NULL);
  got = dbm_firstkey(NULL);
  ASSERT(got.dptr == NULL);
  got = dbm_nextkey(NULL);
  ASSERT(got.dptr == NULL);
  ASSERT_EQ(-1, dbm_delete(NULL, k1));
  ASSERT_EQ(-1, dbm_store(NULL, k1, v1, DBM_INSERT));
  ASSERT_EQ(0, dbm_error(NULL));
  ASSERT_EQ(0, dbm_clearerr(NULL));
  ASSERT_EQ(-1, dbm_dirfno(NULL));

  dbm = dbm_open("memdb", 0, 0600);
  ASSERT(dbm != NULL);
  ASSERT_EQ(-1, dbm_dirfno(dbm));
  got = dbm_firstkey(dbm);
  ASSERT(got.dptr == NULL);

  ASSERT_EQ(0, dbm_store(dbm, k1, v1, DBM_INSERT));
  ASSERT_EQ(1, dbm_store(dbm, k1, v2, DBM_INSERT));
  ASSERT_EQ(0, dbm_store(dbm, k1, v2, DBM_REPLACE));
  ASSERT_EQ(0, dbm_store(dbm, k2, v1, DBM_INSERT));

  got = dbm_fetch(dbm, k1);
  ASSERT_STR_EQ("two", got.dptr);
  got = dbm_fetch(dbm, miss);
  ASSERT(got.dptr == NULL);

  got = dbm_firstkey(dbm);
  ASSERT(got.dptr != NULL);
  got = dbm_nextkey(dbm);
  ASSERT(got.dptr != NULL);
  got = dbm_nextkey(dbm);
  ASSERT(got.dptr == NULL);

  ASSERT_EQ(-1, dbm_delete(dbm, miss));
  ASSERT_EQ(0, dbm_delete(dbm, k1));
  dbm_close(dbm);

  /* OOM tests via allocator hooks */
  g_bsd_db_calloc = mock_db_calloc;
  g_bsd_db_malloc = mock_db_malloc;
  g_bsd_db_realloc = mock_db_realloc;

  /* dbopen: 1st calloc fails, then 2nd calloc fails */
  g_db_calloc_fail_at = 0;
  ASSERT(dbopen(NULL, 0, 0600, DB_HASH, NULL) == NULL);
  g_db_calloc_fail_at = 1;
  ASSERT(dbopen(NULL, 0, 0600, DB_HASH, NULL) == NULL);
  g_db_calloc_fail_at = -1;

  db = dbopen(NULL, 0, 0600, DB_HASH, NULL);
  ASSERT(db != NULL);
  dk.data = "k";
  dk.size = 2;
  dv.data = "v";
  dv.size = 2;

  /* put: realloc fails when growing from 0 -> 16 */
  g_db_realloc_fail = 1;
  ASSERT_EQ(RET_ERROR, db->put(db, &dk, &dv, 0));
  g_db_realloc_fail = 0;

  /* put: nk malloc fails (fail_at = 0), then nv malloc fails (fail_at = 1) */
  g_db_malloc_fail_at = 0;
  ASSERT_EQ(RET_ERROR, db->put(db, &dk, &dv, 0));
  g_db_malloc_fail_at = 1;
  ASSERT_EQ(RET_ERROR, db->put(db, &dk, &dv, 0));
  g_db_malloc_fail_at = -1;

  /* Insert key, then fail malloc on overwrite */
  ASSERT_EQ(RET_SUCCESS, db->put(db, &dk, &dv, 0));
  g_db_malloc_fail_at = 0;
  ASSERT_EQ(RET_ERROR, db->put(db, &dk, &dv, 0));
  g_db_malloc_fail_at = -1;

  bsd_db_reset_alloc_hooks();
  ASSERT_EQ(RET_SUCCESS, db->close(db));

  PASS();
}

SUITE(suite_bsd_db_core) {
  RUN_TEST(test_bsd_db_methods_and_cursor);
  RUN_TEST(test_bsd_ndbm_and_oom);
}
