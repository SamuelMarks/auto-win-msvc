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

static char g_fts_p0[] = "";
static char g_fts_p1[] = "a/b\\c";
static char g_fts_p2[] = "a\\b/c";
static char g_fts_p3[] = "a/b";
static char g_fts_p4[] = "a\\b";
static char g_fts_p5[] = "plain";
static char g_fts_p6[] = "nonexistent_fts_entry_99999";
static char g_fts_p7[] = "ftssbox/";
static char g_fts_p8[] = "ftsempty";
static char g_fts_p9[] = "ftssbox";
static char g_fts_p10[] = "virtual_bs\\";

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
    return opendir("ftssbox");
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
#if defined(__WATCOMC__) && (defined(__DOS__) || defined(__MSDOS__))
  if (_osmajor < 2) {
    SKIP();
  }
#endif
  remove("ftssbox/alpha.txt");
  remove("ftssbox/beta.txt");
  FTS_TEST_RMDIR("ftssbox");
  FTS_TEST_RMDIR("ftsempty");

  ASSERT_EQ(0, FTS_TEST_MKDIR("ftssbox"));
  ASSERT_EQ(0, FTS_TEST_MKDIR("ftsempty"));
  create_empty_file("ftssbox/beta.txt");
  create_empty_file("ftssbox/alpha.txt");

  /* 1. Invalid open / NULL argument paths */
  ASSERT(fts_open(NULL, FTS_PHYSICAL, NULL) == NULL);
  paths[0] = NULL;
  ASSERT(fts_open(paths, FTS_PHYSICAL, NULL) == NULL);
  paths[0] = g_fts_p0;
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
  {
    char long_root[520];
    char root_slash[2];
    char drive_fwd[4];
    char drive_bwd[4];
    char short_sep[4];
    char *sep_paths[6];
    FTS *sep_fts;
    memset(long_root, 'a', sizeof(long_root) - 1);
    long_root[sizeof(long_root) - 1] = '\0';
    memcpy(root_slash, "/", 2);
    memcpy(drive_fwd, "C:/", 4);
    memcpy(drive_bwd, "C:\\", 4);
    memcpy(short_sep, "ab/", 4);
    sep_paths[0] = long_root;
    sep_paths[1] = root_slash;
    sep_paths[2] = drive_fwd;
    sep_paths[3] = drive_bwd;
    sep_paths[4] = short_sep;
    sep_paths[5] = NULL;
    sep_fts = fts_open(sep_paths, FTS_PHYSICAL | FTS_NOSTAT, NULL);
    ASSERT(sep_fts != NULL);
    ASSERT_EQ(0, fts_close(sep_fts));
  }
  paths[0] = g_fts_p1;
  paths[1] = g_fts_p2;
  paths[2] = g_fts_p3;
  paths[3] = g_fts_p4;
  paths[4] = g_fts_p5;
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
  paths[0] = g_fts_p6;
  paths[1] = NULL;
  ftsp = fts_open(paths, FTS_PHYSICAL, NULL);
  ASSERT(ftsp != NULL);
  p = fts_read(ftsp);
  ASSERT(p != NULL);
  ASSERT_EQ(FTS_NS, p->fts_info);
  ASSERT(fts_read(ftsp) == NULL);
  ASSERT_EQ(0, fts_close(ftsp));

  /* 4. Ascending comparator + trailing '/' + FTS_SEEDOT + empty dir */
  paths[0] = g_fts_p7;
  paths[1] = g_fts_p8;
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
  paths[0] = g_fts_p9;
  paths[1] = NULL;
  ftsp = fts_open(paths, FTS_PHYSICAL, fts_cmp_desc);
  ASSERT(ftsp != NULL);
  p = fts_read(ftsp);
  ASSERT(p != NULL);
  ASSERT_EQ(FTS_D, p->fts_info);
  p = fts_read(ftsp);
  ASSERT(p != NULL);
  ASSERT(strcmp(p->fts_name, "beta.txt") == 0 ||
         strcmp(p->fts_name, "BETA.TXT") == 0);
  ASSERT_EQ(FTS_F, p->fts_info);
  while (fts_read(ftsp) != NULL) {
  }
  ASSERT_EQ(0, fts_close(ftsp));

  /* 6. Traversal without comparator (count > 1 && fts_compar == NULL) +
   * empty dir without FTS_SEEDOT (last == NULL when appending dp_ent) */
  paths[0] = g_fts_p9;
  paths[1] = g_fts_p8;
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

#if defined(__WATCOMC__) && (defined(__DOS__) || defined(__MSDOS__))
  if (_osmajor < 2) {
    SKIP();
  }
#endif

  g_bsd_fts_calloc = mock_fts_calloc;
  g_bsd_fts_malloc = mock_fts_malloc;
  g_bsd_fts_opendir = mock_fts_opendir;

  /* 1. FTS_DNR when opendir fails */
  g_fts_opendir_mode = 1;
  paths[0] = g_fts_p9;
  paths[1] = NULL;
  ftsp = fts_open(paths, FTS_PHYSICAL, NULL);
  ASSERT(ftsp != NULL);
  p = fts_read(ftsp);
  ASSERT(p != NULL);
  ASSERT(fts_read(ftsp) == NULL);
  ASSERT_EQ(0, fts_close(ftsp));

  /* 2. Trailing backslash path expansion via redirected opendir */
  g_fts_opendir_mode = 2;
  paths[0] = g_fts_p10;
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
  paths[0] = g_fts_p9;
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
  paths[0] = g_fts_p8;
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

  remove("ftssbox/alpha.txt");
  remove("ftssbox/beta.txt");
  FTS_TEST_RMDIR("ftssbox");
  FTS_TEST_RMDIR("ftsempty");

  PASS();
}

SUITE(suite_bsd_fts_core) {
  RUN_TEST(test_bsd_fts_walk_and_options);
  RUN_TEST(test_bsd_fts_hooks_and_oom);
}
