/* clang-format off */
#include "greatest.h"
#include <fts.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32) || defined(__DOS__) || defined(__MSDOS__) ||               \
    defined(__WATCOMC__)
#include <direct.h>
#define FTS_TEST_MKDIR(p) _mkdir(p)
#define FTS_TEST_RMDIR(p) _rmdir(p)
#else
#include <sys/stat.h>
#include <unistd.h>
#define FTS_TEST_MKDIR(p) mkdir((p), 0755)
#define FTS_TEST_RMDIR(p) rmdir(p)
#endif
/* clang-format on */

extern void *(*g_bsd_fts_calloc)(size_t, size_t);
extern void *(*g_bsd_fts_malloc)(size_t);
extern DIR *(*g_bsd_fts_opendir)(const char *);
extern void bsd_fts_reset_hooks(void);

static int g_fts_calloc_fail_at = -1;
static int g_fts_malloc_fail_at = -1;
static int g_fts_opendir_mode = 0;

static void *mock_fts_calloc(size_t nmemb, size_t size) {
  if (g_fts_calloc_fail_at == 0) {
    return NULL;
  }
  if (g_fts_calloc_fail_at > 0) {
    g_fts_calloc_fail_at--;
  }
  return calloc(nmemb, size);
}

static void *mock_fts_malloc(size_t size) {
  if (g_fts_malloc_fail_at == 0) {
    return NULL;
  }
  if (g_fts_malloc_fail_at > 0) {
    g_fts_malloc_fail_at--;
  }
  return malloc(size);
}

static DIR *mock_fts_opendir(const char *name) {
  if (g_fts_opendir_mode == 1) {
    errno = EACCES;
    return NULL;
  }
  if (g_fts_opendir_mode == 2) {
    return opendir("test_fts_sandbox");
  }
  return opendir(name);
}

static int fts_cmp_asc(const FTSENT *const *a, const FTSENT *const *b) {
  return strcmp((*a)->fts_name, (*b)->fts_name);
}

static int fts_cmp_desc(const FTSENT *const *a, const FTSENT *const *b) {
  return strcmp((*b)->fts_name, (*a)->fts_name);
}

static void create_empty_file(const char *path) {
  FILE *fp;
#if defined(_MSC_VER)
  if (fopen_s(&fp, path, "wb") != 0) {
    fp = NULL;
  }
#else
  fp = fopen(path, "wb");
#endif
  if (fp != NULL) {
    fclose(fp);
  }
}

TEST test_bsd_fts_walk_and_options(void) {
  char *paths[7];
  FTS *ftsp;
  FTSENT *p;
  FTSENT *ch;
  int tag = 77;
  int seen_dp = 0;
  int seen_dot = 0;

  /* Prepare controlled sandbox directory + empty directory */
  remove("test_fts_sandbox/alpha.txt");
  remove("test_fts_sandbox/beta.txt");
  FTS_TEST_RMDIR("test_fts_sandbox");
  FTS_TEST_RMDIR("test_fts_empty");

  ASSERT_EQ(0, FTS_TEST_MKDIR("test_fts_sandbox"));
  ASSERT_EQ(0, FTS_TEST_MKDIR("test_fts_empty"));
  create_empty_file("test_fts_sandbox/beta.txt");
  create_empty_file("test_fts_sandbox/alpha.txt");

  /* 1. Invalid open / NULL argument paths */
  ASSERT(fts_open(NULL, FTS_PHYSICAL, NULL) == NULL);
  paths[0] = NULL;
  ASSERT(fts_open(paths, FTS_PHYSICAL, NULL) == NULL);
  paths[0] = "";
  paths[1] = NULL;
  ASSERT(fts_open(paths, FTS_PHYSICAL, NULL) == NULL);

  ASSERT(fts_read(NULL) == NULL);
  ASSERT(fts_children(NULL, 0) == NULL);
  ASSERT_EQ(-1, fts_set(NULL, NULL, FTS_SKIP));
  ASSERT_EQ(-1, fts_close(NULL));
  ASSERT(fts_get_clientptr(NULL) == NULL);
  ASSERT(fts_get_stream(NULL) == NULL);
  fts_set_clientptr(NULL, &tag);

  /* 2. Multiple roots covering slash/backslash base parsing, FTS_NOSTAT,
   * clientptr, fts_children, pre-read FTS_SKIP, and mid-traversal fts_close */
  paths[0] = "a/b\\c";
  paths[1] = "a\\b/c";
  paths[2] = "a/b";
  paths[3] = "a\\b";
  paths[4] = "plain";
  paths[5] = NULL;
  ftsp = fts_open(paths, FTS_PHYSICAL | FTS_NOSTAT, NULL);
  ASSERT(ftsp != NULL);
  fts_set_clientptr(ftsp, &tag);
  ASSERT(fts_get_clientptr(ftsp) == &tag);

  ch = fts_children(ftsp, 0);
  ASSERT(ch != NULL);
  ASSERT_EQ(0, fts_set(ftsp, ch, FTS_SKIP));
  /* First child was marked FTS_SKIP -> fts_read skips it and returns 2nd */
  p = fts_read(ftsp);
  ASSERT(p != NULL);
  ASSERT_EQ(FTS_NSOK, p->fts_info);
  ASSERT(fts_get_stream(p) == ftsp);
  /* Close mid-traversal while both fts_cur and fts_child are non-NULL */
  ASSERT_EQ(0, fts_close(ftsp));

  /* 3. Nonexistent path without FTS_NOSTAT -> FTS_NS */
  paths[0] = "nonexistent_fts_entry_99999";
  paths[1] = NULL;
  ftsp = fts_open(paths, FTS_PHYSICAL, NULL);
  ASSERT(ftsp != NULL);
  p = fts_read(ftsp);
  ASSERT(p != NULL);
  ASSERT_EQ(FTS_NS, p->fts_info);
  ASSERT(fts_read(ftsp) == NULL);
  ASSERT_EQ(0, fts_close(ftsp));

  /* 4. Ascending comparator + trailing '/' + FTS_SEEDOT + empty dir */
  paths[0] = "test_fts_sandbox/";
  paths[1] = "test_fts_empty";
  paths[2] = NULL;
  ftsp = fts_open(paths, FTS_PHYSICAL | FTS_SEEDOT, fts_cmp_asc);
  ASSERT(ftsp != NULL);
  while ((p = fts_read(ftsp)) != NULL) {
    if (p->fts_info == FTS_DP) {
      seen_dp++;
    }
    if (strcmp(p->fts_name, ".") == 0) {
      seen_dot++;
    }
    if (p->fts_level >= 1 && p->fts_info == FTS_D) {
      ASSERT_EQ(0, fts_set(ftsp, p, FTS_SKIP));
    }
  }
  ASSERT_EQ(2, seen_dp);
  ASSERT(seen_dot >= 1);
  ASSERT_EQ(0, fts_close(ftsp));

  /* 5. Descending comparator + no trailing slash (guaranteed opposite sort
   * order) */
  paths[0] = "test_fts_sandbox";
  paths[1] = NULL;
  ftsp = fts_open(paths, FTS_PHYSICAL, fts_cmp_desc);
  ASSERT(ftsp != NULL);
  p = fts_read(ftsp);
  ASSERT(p != NULL);
  ASSERT_EQ(FTS_D, p->fts_info);
  p = fts_read(ftsp);
  ASSERT(p != NULL);
  ASSERT_STR_EQ("beta.txt", p->fts_name);
  ASSERT_EQ(FTS_F, p->fts_info);
  while (fts_read(ftsp) != NULL) {
  }
  ASSERT_EQ(0, fts_close(ftsp));

  /* 6. Traversal without comparator (count > 1 && fts_compar == NULL) +
   * empty dir without FTS_SEEDOT (last == NULL when appending dp_ent) */
  paths[0] = "test_fts_sandbox";
  paths[1] = "test_fts_empty";
  paths[2] = NULL;
  ftsp = fts_open(paths, FTS_PHYSICAL, NULL);
  ASSERT(ftsp != NULL);
  while (fts_read(ftsp) != NULL) {
  }
  ASSERT_EQ(0, fts_close(ftsp));

  PASS();
}

TEST test_bsd_fts_hooks_and_oom(void) {
  char *paths[3];
  FTS *ftsp;
  FTSENT *p;

  g_bsd_fts_calloc = mock_fts_calloc;
  g_bsd_fts_malloc = mock_fts_malloc;
  g_bsd_fts_opendir = mock_fts_opendir;

  /* 1. FTS_DNR when opendir fails */
  g_fts_opendir_mode = 1;
  paths[0] = "test_fts_sandbox";
  paths[1] = NULL;
  ftsp = fts_open(paths, FTS_PHYSICAL, NULL);
  ASSERT(ftsp != NULL);
  p = fts_read(ftsp);
  ASSERT(p != NULL);
  ASSERT(fts_read(ftsp) == NULL);
  ASSERT_EQ(0, fts_close(ftsp));

  /* 2. Trailing backslash path expansion via redirected opendir */
  g_fts_opendir_mode = 2;
  paths[0] = "virtual_bs\\";
  paths[1] = NULL;
  ftsp = fts_open(paths, FTS_PHYSICAL | FTS_NOSTAT, NULL);
  ASSERT(ftsp != NULL);
  p = fts_read(ftsp);
  ASSERT(p != NULL);
  p->fts_info = FTS_D;
  while (fts_read(ftsp) != NULL) {
  }
  ASSERT_EQ(0, fts_close(ftsp));
  g_fts_opendir_mode = 0;

  /* 3. OOM in fts_open: sp calloc fails */
  g_fts_calloc_fail_at = 0;
  paths[0] = "test_fts_sandbox";
  paths[1] = NULL;
  ASSERT(fts_open(paths, FTS_PHYSICAL, NULL) == NULL);
  g_fts_calloc_fail_at = -1;

  /* 4. OOM in fts_open: root entry calloc fails -> sp has empty fts_child */
  g_fts_calloc_fail_at = 1;
  ftsp = fts_open(paths, FTS_PHYSICAL, NULL);
  ASSERT(ftsp != NULL);
  ASSERT(fts_read(ftsp) == NULL);
  ASSERT_EQ(0, fts_close(ftsp));
  g_fts_calloc_fail_at = -1;

  /* 5. OOM in fts_alloc_ent: fts_path malloc fails */
  g_fts_malloc_fail_at = 0;
  ftsp = fts_open(paths, FTS_PHYSICAL, NULL);
  ASSERT(ftsp != NULL);
  ASSERT(fts_read(ftsp) == NULL);
  ASSERT_EQ(0, fts_close(ftsp));
  g_fts_malloc_fail_at = -1;

  /* 6. OOM in fts_expand_dir: childpath malloc fails on 1st child */
  ftsp = fts_open(paths, FTS_PHYSICAL, NULL);
  ASSERT(ftsp != NULL);
  p = fts_read(ftsp);
  ASSERT(p != NULL);
  g_fts_malloc_fail_at = 0;
  while (fts_read(ftsp) != NULL) {
  }
  g_fts_malloc_fail_at = -1;
  ASSERT_EQ(0, fts_close(ftsp));

  /* 7. OOM in fts_expand_dir: child FTSENT calloc fails */
  ftsp = fts_open(paths, FTS_PHYSICAL, NULL);
  ASSERT(ftsp != NULL);
  p = fts_read(ftsp);
  ASSERT(p != NULL);
  g_fts_calloc_fail_at = 0;
  while (fts_read(ftsp) != NULL) {
  }
  g_fts_calloc_fail_at = -1;
  ASSERT_EQ(0, fts_close(ftsp));

  /* 8. OOM in fts_expand_dir: sort array malloc fails (2 children + compar) */
  ftsp = fts_open(paths, FTS_PHYSICAL, fts_cmp_asc);
  ASSERT(ftsp != NULL);
  p = fts_read(ftsp);
  ASSERT(p != NULL);
  /* 2 children each do 2 mallocs (childpath + fts_path) = 4 mallocs; 5th is arr
   */
  g_fts_malloc_fail_at = 4;
  while (fts_read(ftsp) != NULL) {
  }
  g_fts_malloc_fail_at = -1;
  ASSERT_EQ(0, fts_close(ftsp));

  /* 9. OOM in fts_expand_dir: dp_ent calloc fails on non-empty dir (first !=
   * NULL) */
  ftsp = fts_open(paths, FTS_PHYSICAL, NULL);
  ASSERT(ftsp != NULL);
  p = fts_read(ftsp);
  ASSERT(p != NULL);
  g_fts_calloc_fail_at = 2;
  while (fts_read(ftsp) != NULL) {
  }
  g_fts_calloc_fail_at = -1;
  ASSERT_EQ(0, fts_close(ftsp));

  /* 10. OOM in fts_expand_dir: dp_ent calloc fails on empty dir (first == NULL)
   */
  paths[0] = "test_fts_empty";
  paths[1] = NULL;
  ftsp = fts_open(paths, FTS_PHYSICAL, NULL);
  ASSERT(ftsp != NULL);
  p = fts_read(ftsp);
  ASSERT(p != NULL);
  g_fts_calloc_fail_at = 0;
  ASSERT(fts_read(ftsp) == NULL);
  g_fts_calloc_fail_at = -1;
  ASSERT_EQ(0, fts_close(ftsp));

  bsd_fts_reset_hooks();

  remove("test_fts_sandbox/alpha.txt");
  remove("test_fts_sandbox/beta.txt");
  FTS_TEST_RMDIR("test_fts_sandbox");
  FTS_TEST_RMDIR("test_fts_empty");

  PASS();
}

SUITE(suite_bsd_fts_core) {
  RUN_TEST(test_bsd_fts_walk_and_options);
  RUN_TEST(test_bsd_fts_hooks_and_oom);
}
