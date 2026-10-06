#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-glob.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#if defined(_WIN32)
#include <direct.h>
#define MKDIR(d) _mkdir(d)
#define RMDIR(d) _rmdir(d)
#else
#include <sys/stat.h>
#include <unistd.h>
#define MKDIR(d) mkdir(d, 0777)
#define RMDIR(d) rmdir(d)
#endif


#define POSIX_GLOB_MOCK_MALLOC 1
static int g_malloc_fail_at = -1;
static int g_malloc_count = 0;
static int g_realloc_fail_at = -1;
static int g_realloc_count = 0;

void *posix_glob_mock_malloc(size_t size) {
    if (g_malloc_count++ == g_malloc_fail_at) return NULL;
    return malloc(size);
}
void *posix_glob_mock_realloc(void *ptr, size_t size) {
    if (g_realloc_count++ == g_realloc_fail_at) return NULL;
    return realloc(ptr, size);
}


#if defined(_WIN32)
__declspec(dllimport) extern void *(*posix_glob_mock_malloc_ptr)(size_t);
__declspec(dllimport) extern void *(*posix_glob_mock_realloc_ptr)(void *, size_t);
#else
extern void *(*posix_glob_mock_malloc_ptr)(size_t);
extern void *(*posix_glob_mock_realloc_ptr)(void *, size_t);
#endif

/* clang-format on */
extern int dummy_posix_glob(void);
extern int posix_glob_compare_test_internal(const void *a, const void *b);
extern int dummy_posix_glob(void);
extern int posix_glob_compare_test_internal(const void *a, const void *b);

static int errfunc_dummy(const char *epath, int eerrno) {
  if (epath && eerrno) {
    (void)epath;
    (void)eerrno;
  }
  return 0;
}

static int errfunc_abort(const char *epath, int eerrno) {
  (void)epath;
  (void)eerrno;
  return 1;
}

TEST test_posix_glob_init(void) {
  auto_win_msvc_error_t rc;
  int status;

  status = 0;
  rc = posix_glob_get_info(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_glob_get_info(&status);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);

  ASSERT_EQ(1, status);

  dummy_posix_glob();

  PASS();
}

TEST test_fnmatch(void) {
  ASSERT_EQ(0, fnmatch("*.txt", "foo.txt", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("*.txt", "foo.doc", 0));

  ASSERT_EQ(0, fnmatch("foo?bar", "fooXbar", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("foo?bar", "foobar", 0));

  ASSERT_EQ(0, fnmatch("a\\*b", "a*b", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("a\\*b", "aXb", 0));

  ASSERT_EQ(FNM_NOMATCH, fnmatch("*.txt", "dir/foo.txt", FNM_PATHNAME));
  ASSERT_EQ(0, fnmatch("dir/*.txt", "dir/foo.txt", FNM_PATHNAME));

  ASSERT_EQ(0, fnmatch("*a", "a", 0));
  ASSERT_EQ(0, fnmatch("a*", "a", 0));
  ASSERT_EQ(0, fnmatch("**", "a", 0));

  ASSERT_EQ(0, fnmatch("*dir/foo.txt", "dir/foo.txt", FNM_PATHNAME));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("*foo.txt", "dir/foo.txt", FNM_PATHNAME));

  ASSERT_EQ(FNM_NOMATCH, fnmatch("a*", "a/b", FNM_PATHNAME));

  ASSERT_EQ(0, fnmatch("*", ".hidden", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("*", ".hidden", FNM_PERIOD));
  ASSERT_EQ(0, fnmatch("*", "ahidden", FNM_PERIOD));
  ASSERT_EQ(0, fnmatch("a*", "a.hidden", FNM_PERIOD));

  ASSERT_EQ(FNM_NOMATCH, fnmatch(NULL, "a", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("a", NULL, 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("?", "/", FNM_PATHNAME));
  ASSERT_EQ(0, fnmatch("?", "/", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("?", ".", FNM_PERIOD));
  ASSERT_EQ(0, fnmatch("?", ".", 0));

  /* Missing branches for ? */
  ASSERT_EQ(FNM_NOMATCH, fnmatch("a/?/b", "a///b", FNM_PATHNAME));
  ASSERT_EQ(0, fnmatch("?", "a", FNM_PATHNAME));
  ASSERT_EQ(0, fnmatch("a?", "a.", FNM_PERIOD));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("?", ".b", FNM_PERIOD));
  ASSERT_EQ(0, fnmatch("?", "b", FNM_PERIOD));
  ASSERT_EQ(0, fnmatch("a*", "ab", FNM_PATHNAME));

  ASSERT_EQ(0, fnmatch("\\*", "\\*", FNM_NOESCAPE));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("\\", "a", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("?", "", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("?a", "a", 0));

  PASS();
}

TEST test_glob_basic(void) {
  glob_t g;
  int rc;
  FILE *f;

  MKDIR("glob_test_dir");
#if defined(_MSC_VER)
  fopen_s(&f, "glob_test_dir/file1.txt", "w");
#else
  f = fopen("glob_test_dir/file1.txt", "w");
#endif
  if (f) {
    fclose(f);
  }
#if defined(_MSC_VER)
  fopen_s(&f, "glob_test_dir/file2.txt", "w");
#else
  f = fopen("glob_test_dir/file2.txt", "w");
#endif
  if (f) {
    fclose(f);
  }

  rc = glob("glob_test_dir/*.txt", 0, NULL, &g);
  ASSERT_EQ(0, rc);
  ASSERT_EQ(2, g.gl_pathc);
  ASSERT(g.gl_pathv != NULL);
  globfree(&g);

#if defined(_MSC_VER)
  fopen_s(&f, "file_no_prefix.txt", "w");
#else
  f = fopen("file_no_prefix.txt", "w");
#endif
  if (f)
    fclose(f);
  rc = glob("file_no_prefix.*", 0, NULL, &g);
  ASSERT_EQ(0, rc);
  globfree(&g);
  remove("file_no_prefix.txt");

  rc = glob("/*", 0, NULL, &g);
  ASSERT_EQ(0, rc);
  globfree(&g);

  {
    char long_pat[1500];
    memset(long_pat, 'a', 1400);
    long_pat[1400] = '/';
    long_pat[1401] = '*';
    long_pat[1402] = '\0';
    rc = glob(long_pat, 0, NULL, &g);
    ASSERT(rc == GLOB_NOMATCH || rc == GLOB_NOSPACE || rc == GLOB_ABORTED);
  }

  /* GLOB_APPEND */
  rc = glob("glob_test_dir/file1.txt", 0, NULL, &g);
  ASSERT_EQ(0, rc);
  rc = glob("glob_test_dir/file2.txt", GLOB_APPEND, NULL, &g);
  ASSERT_EQ(0, rc);
  ASSERT_EQ(2, g.gl_pathc);
  globfree(&g);

  /* GLOB_DOOFFS */
  g.gl_offs = 2;
  rc = glob("glob_test_dir/*.txt", GLOB_DOOFFS, NULL, &g);
  ASSERT_EQ(0, rc);
  ASSERT_EQ(2, g.gl_pathc);
  ASSERT_EQ(NULL, g.gl_pathv[0]);
  ASSERT_EQ(NULL, g.gl_pathv[1]);
  ASSERT(g.gl_pathv[2] != NULL);
  globfree(&g);

  /* GLOB_NOCHECK */
  rc = glob("glob_test_dir/nonexistent*.txt", GLOB_NOCHECK, NULL, &g);
  ASSERT_EQ(0, rc);
  ASSERT_EQ(1, g.gl_pathc);
  ASSERT_STR_EQ("glob_test_dir/nonexistent*.txt", g.gl_pathv[0]);
  globfree(&g);

  /* GLOB_MARK */
  MKDIR("glob_test_dir/subdir");
  rc = glob("glob_test_dir/subdir", GLOB_MARK, NULL, &g);
  ASSERT_EQ(0, rc);
  ASSERT_EQ(1, g.gl_pathc);
  ASSERT(strstr(g.gl_pathv[0], "glob_test_dir/subdir") != NULL);
  globfree(&g);

  rc = glob("glob_test_dir/*.txt", GLOB_NOSORT, NULL, &g);
  ASSERT_EQ(0, rc);
  globfree(&g);

  /* GLOB_NOMAGIC with magic chars and no match */
  rc = glob("glob_test_dir/nonexistent*.txt", GLOB_NOMAGIC, NULL, &g);
  ASSERT_EQ(GLOB_NOMATCH, rc);

  /* GLOB_NOMAGIC with no magic chars and no match */
  rc = glob("glob_test_dir/nonexistent.txt", GLOB_NOMAGIC, NULL, &g);
  ASSERT_EQ(0, rc);
  ASSERT_EQ(1, g.gl_pathc);
  ASSERT_STR_EQ("glob_test_dir/nonexistent.txt", g.gl_pathv[0]);
  globfree(&g);

  /* Invalid args */
  ASSERT_EQ(GLOB_ABORTED, glob(NULL, 0, NULL, &g));
  ASSERT_EQ(GLOB_ABORTED, glob("*.txt", 0, NULL, NULL));

  rc = glob("glob_test_dir/file1.txt", GLOB_NOMAGIC, NULL, &g);
  ASSERT_EQ(0, rc);
  globfree(&g);
  globfree(NULL);

  memset(&g, 0, sizeof(g));
  globfree(&g); /* test gl_pathv == NULL */

  /* Cleanup */
  remove("glob_test_dir/file1.txt");
  remove("glob_test_dir/file2.txt");
  RMDIR("glob_test_dir/subdir");
  RMDIR("glob_test_dir");

  PASS();
}

TEST test_glob_errfunc(void) {
  glob_t g;
  int rc;
  rc = glob("non_existent_dir_glob/*", 0, errfunc_dummy, &g);
  ASSERT_EQ(GLOB_NOMATCH, rc);

  rc = glob("non_existent_dir_glob/*", GLOB_ERR, errfunc_abort, &g);
  ASSERT_EQ(GLOB_ABORTED, rc);

  rc = glob("non_existent_dir_glob/*", GLOB_ERR, NULL, &g);
  ASSERT_EQ(GLOB_ABORTED, rc);

  PASS();
}

TEST test_posix_glob_compare_test_internal_branches(void) {
  const char *a = "apple";
  const char *b = "banana";
  const char *null_str = NULL;

  /* we will invoke the internal posix_glob_compare_test_internal directly since
   * we included posix-glob.c */
  ASSERT(posix_glob_compare_test_internal(&a, &b) < 0);
  ASSERT(posix_glob_compare_test_internal(&b, &a) > 0);
  /* ASSERT(posix_glob_compare_test_internal(NULL, &b) == 0); -> crashes */
  ASSERT(posix_glob_compare_test_internal(&null_str, &null_str) == 0);
  ASSERT(posix_glob_compare_test_internal(&null_str, &b) == 1);
  ASSERT(posix_glob_compare_test_internal(&a, &null_str) == -1);

  PASS();
}

TEST test_wordexp_wordfree(void) {
  wordexp_t w;
  int rc;

  rc = wordexp(NULL, NULL, 0);
  ASSERT_EQ(WRDE_SYNTAX, rc);
  rc = wordexp("hello", NULL, 0);
  ASSERT_EQ(WRDE_SYNTAX, rc);

  rc = wordexp("hello world", &w, 0);
  ASSERT_EQ(0, rc);
  ASSERT_EQ(2, w.we_wordc);
  ASSERT_STR_EQ("hello", w.we_wordv[0]);
  ASSERT_STR_EQ("world", w.we_wordv[1]);
  ASSERT_EQ(NULL, w.we_wordv[2]);

  rc = wordexp("appended", &w, WRDE_APPEND);
  ASSERT_EQ(0, rc);
  ASSERT_EQ(3, w.we_wordc);
  ASSERT_STR_EQ("appended", w.we_wordv[2]);

  wordfree(&w);
  wordfree(NULL);
  memset(&w, 0, sizeof(w));
  wordfree(&w); /* test we_wordv == NULL */

  w.we_offs = 2;
  rc = wordexp("offset mode", &w, WRDE_DOOFFS);
  ASSERT_EQ(0, rc);
  ASSERT_EQ(2, w.we_wordc);
  ASSERT_EQ(NULL, w.we_wordv[0]);
  ASSERT_EQ(NULL, w.we_wordv[1]);
  ASSERT_STR_EQ("offset", w.we_wordv[2]);
  ASSERT_STR_EQ("mode", w.we_wordv[3]);

  wordfree(&w);
  PASS();
}

TEST test_glob_oom(void) {
  glob_t g;
  int rc;
  wordexp_t w;
  int i;
  char fname[64];
  FILE *f;

  MKDIR("glob_test_oom");
  for (i = 0; i < 15; i++) {
#if defined(_MSC_VER)
    sprintf_s(fname, sizeof(fname), "glob_test_oom/file%d.txt", i);
#else
    sprintf(fname, "glob_test_oom/file%d.txt", i);
#endif
#if defined(_MSC_VER)
    fopen_s(&f, fname, "w");
#else
    f = fopen(fname, "w");
#endif
    if (f)
      fclose(f);
  }

  posix_glob_mock_malloc_ptr = posix_glob_mock_malloc;
  posix_glob_mock_realloc_ptr = posix_glob_mock_realloc;

  g_malloc_count = 0;
  g_malloc_fail_at = 0;
  rc = glob("glob_test_oom/*.txt", 0, NULL, &g);
  ASSERT_EQ(GLOB_NOSPACE, rc);
  g_malloc_fail_at = -1;

  g_malloc_count = 0;
  g_malloc_fail_at = 1;
  rc = glob("glob_test_oom/*.txt", 0, NULL, &g);
  ASSERT_EQ(GLOB_NOSPACE, rc);
  g_malloc_fail_at = -1;

  g_realloc_count = 0;
  g_realloc_fail_at = 0;
  rc = glob("glob_test_oom/*.txt", 0, NULL, &g);
  ASSERT_EQ(GLOB_NOSPACE, rc);
  g_realloc_fail_at = -1;

  g.gl_offs = 2;
  g_malloc_count = 0;
  g_malloc_fail_at = 0;
  rc = glob("glob_test_oom/*.txt", GLOB_DOOFFS, NULL, &g);
  ASSERT_EQ(GLOB_NOSPACE, rc);
  g_malloc_fail_at = -1;

  g_malloc_count = 0;
  g_malloc_fail_at = 0;
  rc = wordexp("hello world", &w, 0);
  ASSERT_EQ(WRDE_NOSPACE, rc);
  g_malloc_fail_at = -1;

  g_malloc_count = 0;
  g_malloc_fail_at = 1;
  rc = wordexp("hello world", &w, 0);
  ASSERT_EQ(WRDE_NOSPACE, rc);
  g_malloc_fail_at = -1;

  g_realloc_count = 0;
  g_realloc_fail_at = 0;
  rc = wordexp("hello world", &w, 0);
  ASSERT_EQ(WRDE_NOSPACE, rc);
  g_realloc_fail_at = -1;

  w.we_offs = 2;
  g_malloc_count = 0;
  g_malloc_fail_at = 0;
  rc = wordexp("hello world", &w, WRDE_DOOFFS);
  ASSERT_EQ(WRDE_NOSPACE, rc);
  g_malloc_fail_at = -1;

  for (i = 0; i < 15; i++) {
#if defined(_MSC_VER)
    sprintf_s(fname, sizeof(fname), "glob_test_oom/file%d.txt", i);
#else
    sprintf(fname, "glob_test_oom/file%d.txt", i);
#endif
    remove(fname);
  }
  RMDIR("glob_test_oom");

  posix_glob_mock_malloc_ptr = NULL;
  posix_glob_mock_realloc_ptr = NULL;

  PASS();
}

TEST test_glob_long_path(void) {
  glob_t g;
  int rc;
  char pattern[2048];
  memset(pattern, 'a', 2047);
  pattern[2047] = '\0';
  rc = glob(pattern, 0, NULL, &g);
  ASSERT(rc == GLOB_NOMATCH || rc == GLOB_NOSPACE || rc == GLOB_ABORTED);
  globfree(&g);
  PASS();
}

SUITE(suite_posix_glob_core) {
  RUN_TEST(test_posix_glob_init);
  RUN_TEST(test_fnmatch);
  RUN_TEST(test_glob_basic);
  RUN_TEST(test_glob_errfunc);
  RUN_TEST(test_posix_glob_compare_test_internal_branches);
  RUN_TEST(test_wordexp_wordfree);
  RUN_TEST(test_glob_oom);
  RUN_TEST(test_glob_long_path);
}
