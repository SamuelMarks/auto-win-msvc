#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-glob.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

TEST test_posix_glob_get_info(void) {
  enum posix_glob_error_code rc;
  int info;

  info = 0;
  rc = posix_glob_get_info(NULL);
  if (rc != POSIX_GLOB_ERROR_NULL_POINTER) {
    printf("Expected POSIX_GLOB_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_glob_get_info(&info);
  if (rc != POSIX_GLOB_SUCCESS) {
    printf("posix_glob_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_fnmatch_comprehensive(void) {
  /* Null input checks */
  ASSERT_EQ(FNM_NOMATCH, fnmatch(NULL, "hello", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("hello", NULL, 0));

  /* Exact matches */
  ASSERT_EQ(0, fnmatch("hello", "hello", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("hello", "world", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("hello", "hell", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("hell", "hello", 0));

  /* Question mark '?' */
  ASSERT_EQ(0, fnmatch("h?llo", "hello", 0));
  ASSERT_EQ(0, fnmatch("h?llo", "hallo", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("h?llo", "hllo", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("?", "", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("h?llo", "h/llo", FNM_PATHNAME));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("?hello", ".hello", FNM_PERIOD));

  /* Wildcard '*' */
  ASSERT_EQ(0, fnmatch("*.c", "test.c", 0));
  ASSERT_EQ(0, fnmatch("*.c", ".c", 0));
  ASSERT_EQ(0, fnmatch("*", "anything", 0));
  ASSERT_EQ(0, fnmatch("*", "", 0));
  ASSERT_EQ(0, fnmatch("***", "multiple_stars", 0));
  ASSERT_EQ(0, fnmatch("a*b*c", "a_middle_b_end_c", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("*.c", "test.h", 0));

  /* FNM_PATHNAME with '*' */
  ASSERT_EQ(0, fnmatch("dir/*", "dir/file.txt", FNM_PATHNAME));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("*", "dir/file.txt", FNM_PATHNAME));

  /* FNM_PERIOD with '*' */
  ASSERT_EQ(FNM_NOMATCH, fnmatch("*", ".hidden", FNM_PERIOD));
  ASSERT_EQ(0, fnmatch(".*", ".hidden", FNM_PERIOD));

  /* Escape sequences */
  ASSERT_EQ(0, fnmatch("\\*", "*", 0));
  ASSERT_EQ(FNM_NOMATCH, fnmatch("\\*", "a", 0));
  ASSERT_EQ(0, fnmatch("\\*", "\\*", FNM_NOESCAPE));

  PASS();
}

TEST test_glob_comprehensive(void) {
  glob_t g;
  int res;

  /* Null pointer checks */
  ASSERT_EQ(GLOB_ABORTED, glob(NULL, 0, NULL, &g));
  ASSERT_EQ(GLOB_ABORTED, glob("*", 0, NULL, NULL));

  /* Matching files */
  memset(&g, 0, sizeof(g));
  res = glob("*", 0, NULL, &g);
  ASSERT_EQ(0, res);
  ASSERT(g.gl_pathc > 0);
  ASSERT(g.gl_pathv != NULL);
  globfree(&g);

  /* Glob with directory prefix */
  memset(&g, 0, sizeof(g));
  res = glob("./*", 0, NULL, &g);
  ASSERT_EQ(0, res);
  ASSERT(g.gl_pathc > 0);
  globfree(&g);

  /* Non-matching pattern without NOCHECK */
  memset(&g, 0, sizeof(g));
  res = glob("non_existent_file_pattern_12345*.xyz", 0, NULL, &g);
  ASSERT_EQ(GLOB_NOMATCH, res);
  globfree(&g);

  /* Non-matching pattern with NOCHECK */
  memset(&g, 0, sizeof(g));
  res = glob("non_existent_file_pattern_12345*.xyz", GLOB_NOCHECK, NULL, &g);
  ASSERT_EQ(0, res);
  ASSERT_EQ(1, g.gl_pathc);
  globfree(&g);

  /* GLOB_DOOFFS */
  memset(&g, 0, sizeof(g));
  g.gl_offs = 2;
  res = glob("*", GLOB_DOOFFS, NULL, &g);
  ASSERT_EQ(0, res);
  ASSERT(g.gl_pathc > 0);
  ASSERT_EQ(NULL, g.gl_pathv[0]);
  ASSERT_EQ(NULL, g.gl_pathv[1]);
  ASSERT(g.gl_pathv[2] != NULL);
  globfree(&g);

  /* GLOB_APPEND */
  memset(&g, 0, sizeof(g));
  res = glob("*.c", GLOB_NOCHECK, NULL, &g);
  ASSERT_EQ(0, res);
  res = glob("*.h", GLOB_NOCHECK | GLOB_APPEND, NULL, &g);
  ASSERT_EQ(0, res);
  ASSERT(g.gl_pathc >= 2);
  globfree(&g);

  /* Freeing NULL or empty */
  globfree(NULL);
  memset(&g, 0, sizeof(g));
  globfree(&g);

  PASS();
}

TEST test_wordexp_comprehensive(void) {
  wordexp_t we;
  int res;

  /* Null input checks */
  ASSERT_EQ(WRDE_SYNTAX, wordexp(NULL, &we, 0));
  ASSERT_EQ(WRDE_SYNTAX, wordexp("hello", NULL, 0));

  /* Basic word splitting */
  memset(&we, 0, sizeof(we));
  res = wordexp("one two   three\tfour\nfive", &we, 0);
  ASSERT_EQ(0, res);
  ASSERT_EQ(5, we.we_wordc);
  ASSERT_STR_EQ("one", we.we_wordv[0]);
  ASSERT_STR_EQ("two", we.we_wordv[1]);
  ASSERT_STR_EQ("three", we.we_wordv[2]);
  ASSERT_STR_EQ("four", we.we_wordv[3]);
  ASSERT_STR_EQ("five", we.we_wordv[4]);
  ASSERT_EQ(NULL, we.we_wordv[5]);
  wordfree(&we);

  /* WRDE_DOOFFS */
  memset(&we, 0, sizeof(we));
  we.we_offs = 3;
  res = wordexp("alpha beta", &we, WRDE_DOOFFS);
  ASSERT_EQ(0, res);
  ASSERT_EQ(2, we.we_wordc);
  ASSERT_EQ(NULL, we.we_wordv[0]);
  ASSERT_EQ(NULL, we.we_wordv[1]);
  ASSERT_EQ(NULL, we.we_wordv[2]);
  ASSERT_STR_EQ("alpha", we.we_wordv[3]);
  ASSERT_STR_EQ("beta", we.we_wordv[4]);
  wordfree(&we);

  /* WRDE_APPEND */
  memset(&we, 0, sizeof(we));
  res = wordexp("first second", &we, 0);
  ASSERT_EQ(0, res);
  res = wordexp("third", &we, WRDE_APPEND);
  ASSERT_EQ(0, res);
  ASSERT_EQ(3, we.we_wordc);
  ASSERT_STR_EQ("first", we.we_wordv[0]);
  ASSERT_STR_EQ("second", we.we_wordv[1]);
  ASSERT_STR_EQ("third", we.we_wordv[2]);
  wordfree(&we);

  /* Empty string */
  memset(&we, 0, sizeof(we));
  res = wordexp("", &we, 0);
  ASSERT_EQ(0, res);
  ASSERT_EQ(0, we.we_wordc);
  wordfree(&we);

  /* wordfree on NULL */
  wordfree(NULL);
  memset(&we, 0, sizeof(we));
  wordfree(&we);

  PASS();
}

SUITE(suite_posix_glob_core) {
  RUN_TEST(test_posix_glob_get_info);
  RUN_TEST(test_fnmatch_comprehensive);
  RUN_TEST(test_glob_comprehensive);
  RUN_TEST(test_wordexp_comprehensive);
}
