/* clang-format off */
#include "greatest.h"
#include <stringlist.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

extern void *(*g_bsd_sl_calloc)(size_t, size_t);
extern void *(*g_bsd_sl_realloc)(void *, size_t);
extern void bsd_sl_reset_hooks(void);

static int g_sl_fail_after = -1;

static void *mock_sl_calloc(size_t nmemb, size_t size) {
  if (g_sl_fail_after == 0) {
    return NULL;
  }
  if (g_sl_fail_after > 0) {
    g_sl_fail_after--;
  }
  return calloc(nmemb, size);
}

static void *mock_sl_realloc(void *ptr, size_t size) {
  if (g_sl_fail_after == 0) {
    return NULL;
  }
  if (g_sl_fail_after > 0) {
    g_sl_fail_after--;
  }
  return realloc(ptr, size);
}

static char *dup_c89(const char *s) {
  size_t len = strlen(s) + 1;
  char *out = (char *)malloc(len);
  if (out != NULL) {
    memcpy(out, s, len);
  }
  return out;
}

static void
exercise_stringlist_api(StringList *(*fn_init)(void),
                        int (*fn_add)(StringList *, char *),
                        void (*fn_free)(StringList *, int),
                        char *(*fn_find)(StringList *, const char *),
                        int (*fn_del)(StringList *, const char *, int)) {
  StringList *sl;
  char s_x[] = "x";
  char s_alpha[] = "alpha";
  char s_beta[] = "beta";
  int i;

  /* NULL safety */
  fn_free(NULL, 0);
  sl = (StringList *)calloc(1, sizeof(StringList));
  if (sl != NULL) {
    sl->sl_str = NULL;
    fn_free(sl, 1);
  }

  if (fn_add(NULL, s_x) != -1) {
    abort();
  }
  if (fn_find(NULL, "x") != NULL) {
    abort();
  }
  if (fn_del(NULL, "x", 0) != -1) {
    abort();
  }

  sl = fn_init();
  if (sl == NULL) {
    abort();
  }
  if (fn_find(sl, NULL) != NULL) {
    abort();
  }
  if (fn_del(sl, NULL, 0) != -1) {
    abort();
  }

  /* Add NULL element + dynamic elements beyond initial capacity 20 */
  fn_add(sl, NULL);
  for (i = 0; i < 25; i++) {
    fn_add(sl, dup_c89(i == 5 ? "target" : "item"));
  }

  if (fn_find(sl, "target") == NULL) {
    abort();
  }
  if (fn_find(sl, "missing") != NULL) {
    abort();
  }

  /* Delete non-existent */
  if (fn_del(sl, "missing", 0) != -1) {
    abort();
  }
  /* Delete middle element with all=1 (shifts subsequent items) */
  if (fn_del(sl, "target", 1) != 0) {
    abort();
  }
  if (fn_find(sl, "target") != NULL) {
    abort();
  }

  /* Free all remaining heap elements (free(NULL) is standard C89 no-op) */
  fn_free(sl, 1);

  /* Also test sl_delete with all=0 and sl_free with all=0 */
  sl = fn_init();
  fn_add(sl, s_alpha);
  fn_add(sl, s_beta);
  fn_del(sl, "alpha", 0);
  fn_del(sl, "beta", 0);
  fn_free(sl, 0);
}

TEST test_bsd_stringlist_ops(void) {
  bsd_sl_reset_hooks();
  exercise_stringlist_api(sl_init, sl_add, sl_free, sl_find, sl_delete);
  PASS();
}

TEST test_bsd_stringlist_oom(void) {
  StringList *sl;
  char s_x[] = "x";
  char s_ovf[] = "overflow";
  int i;

  g_bsd_sl_calloc = mock_sl_calloc;
  g_bsd_sl_realloc = mock_sl_realloc;

  /* 1. First calloc in sl_init fails */
  g_sl_fail_after = 0;
  ASSERT(sl_init() == NULL);

  /* 2. Second calloc in sl_init fails */
  g_sl_fail_after = 1;
  ASSERT(sl_init() == NULL);

  /* 3. realloc in sl_add fails when growing at item 19 */
  g_sl_fail_after = -1;
  sl = sl_init();
  ASSERT(sl != NULL);
  for (i = 0; i < 19; i++) {
    ASSERT_EQ(0, sl_add(sl, s_x));
  }
  g_sl_fail_after = 0;
  ASSERT_EQ(-1, sl_add(sl, s_ovf));
  g_sl_fail_after = -1;
  sl_free(sl, 0);

  bsd_sl_reset_hooks();

  PASS();
}

SUITE(suite_bsd_stringlist_core) {
  RUN_TEST(test_bsd_stringlist_ops);
  RUN_TEST(test_bsd_stringlist_oom);
}
