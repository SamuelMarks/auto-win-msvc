/* clang-format off */
#include "greatest.h"
#include "posix-regex.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
extern void posix_regex_set_pcre2_hooks(
    void *(*compile_fn)(const char *, unsigned int, int *, size_t *),
    int (*info_fn)(void *, unsigned int *), void *(*md_create_fn)(void *),
    void (*md_free_fn)(void *),
    int (*match_fn)(void *, const char *, size_t, size_t, unsigned int,
                    void *));
extern void posix_regex_reset_pcre2_hooks(void);

static regex_t g_donor_re;
static int g_compile_mode = 0;
static int g_info_ret = 0;
static int g_md_null = 0;
static int g_match_ret = -48; /* PCRE2_ERROR_NOMEMORY */
static int g_dummy_md = 1;

static void *mock_compile(const char *pat, unsigned int opts, int *errcode,
                          size_t *erroff) {
  (void)pat;
  (void)opts;
  *erroff = 0;
  if (g_compile_mode == 0) {
    *errcode = -48; /* PCRE2_ERROR_NOMEMORY */
    return NULL;
  }
  *errcode = 0;
  return g_donor_re.re_guts;
}

static int mock_pattern_info(void *re, unsigned int *out_captures) {
  (void)re;
  *out_captures = 0;
  return g_info_ret;
}

static void *mock_md_create(void *re) {
  (void)re;
  return g_md_null ? NULL : (void *)&g_dummy_md;
}

static void mock_md_free(void *md) { (void)md; }

static int mock_match_oom(void *re, const char *str, size_t len,
                          size_t start_off, unsigned int opts,
                          void *match_data) {
  (void)re;
  (void)str;
  (void)len;
  (void)start_off;
  (void)opts;
  (void)match_data;
  return g_match_ret;
}
#endif

TEST test_posix_regex_init(void) {
  auto_win_msvc_error_t rc;
  int status = 0;

  rc = posix_regex_init(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_regex_init(&status);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, status);
  PASS();
}

TEST test_regcomp_exec(void) {
  regex_t preg;
  regmatch_t pmatch[4];
  int rc;

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
  ASSERT_EQ(REG_INVARG, regcomp(NULL, "abc", 0));
  ASSERT_EQ(REG_INVARG, regcomp(&preg, NULL, 0));
#endif

  /* Invalid pattern syntax -> REG_BADPAT */
  ASSERT_NEQ(0, regcomp(&preg, "[unclosed", REG_EXTENDED));

  rc = regcomp(&preg, "^hello ([a-z]+)$",
               REG_EXTENDED | REG_ICASE | REG_NEWLINE);
  ASSERT_EQ(0, rc);

  rc = regexec(&preg, "HELLO world", 4, pmatch, 0);
  ASSERT_EQ(0, rc);
  ASSERT_EQ(0, (int)pmatch[0].rm_so);
  ASSERT_EQ(11, (int)pmatch[0].rm_eo);
  ASSERT_EQ(6, (int)pmatch[1].rm_so);
  ASSERT_EQ(11, (int)pmatch[1].rm_eo);
  ASSERT_EQ(-1, (int)pmatch[2].rm_so);
  ASSERT_EQ(-1, (int)pmatch[2].rm_eo);

  /* Zero nmatch and non-matching with REG_NOTBOL | REG_NOTEOL */
  ASSERT_EQ(0, regexec(&preg, "HELLO world", 0, NULL, 0));
  ASSERT_EQ(REG_NOMATCH,
            regexec(&preg, "HELLO world", 2, pmatch, REG_NOTBOL | REG_NOTEOL));

  regfree(&preg);
  PASS();
}

TEST test_regexec_startend_and_errors(void) {
  regex_t preg;
  regmatch_t pmatch[2];

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
  ASSERT_EQ(REG_INVARG, regexec(NULL, "abc", 0, NULL, 0));

  memset(&preg, 0, sizeof(preg));
  ASSERT_EQ(REG_INVARG, regexec(&preg, NULL, 0, NULL, 0));
  ASSERT_EQ(REG_INVARG, regexec(&preg, "abc", 0, NULL, 0));
#else
  (void)preg;
  (void)pmatch;
#endif

#ifdef REG_STARTEND
  ASSERT_EQ(0, regcomp(&preg, "world", REG_EXTENDED));

  /* Valid REG_STARTEND */
  pmatch[0].rm_so = 6;
  pmatch[0].rm_eo = 11;
  ASSERT_EQ(0, regexec(&preg, "hello world!", 2, pmatch, REG_STARTEND));

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
  /* REG_STARTEND edge cases falling through to standard match:
   * pmatch == NULL, rm_eo < rm_so, rm_so < 0 */
  ASSERT_EQ(0, regexec(&preg, "hello world!", 0, NULL, REG_STARTEND));

  pmatch[0].rm_so = 8;
  pmatch[0].rm_eo = 3;
  ASSERT_EQ(0, regexec(&preg, "hello world!", 1, pmatch, REG_STARTEND));

  pmatch[0].rm_so = -1;
  pmatch[0].rm_eo = 5;
  ASSERT_EQ(0, regexec(&preg, "hello world!", 1, pmatch, REG_STARTEND));
#endif

  regfree(&preg);
#endif

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
  /* Mock PCRE2 OOM and pattern_info failure branches */
  ASSERT_EQ(0, regcomp(&g_donor_re, "abc", 0));

  posix_regex_set_pcre2_hooks(mock_compile, mock_pattern_info, mock_md_create,
                              mock_md_free, mock_match_oom);
  g_compile_mode = 0;
  ASSERT_EQ(REG_ESPACE, regcomp(&preg, "abc", 0));

  /* pattern_info != 0 frees g_donor_re.re_guts */
  g_compile_mode = 1;
  g_info_ret = -1;
  ASSERT_EQ(REG_BADPAT, regcomp(&preg, "abc", 0));
  g_donor_re.re_guts = NULL;

  posix_regex_reset_pcre2_hooks();
  ASSERT_EQ(0, regcomp(&preg, "abc", 0));

  /* match_data_create == NULL -> REG_ESPACE */
  posix_regex_set_pcre2_hooks(mock_compile, mock_pattern_info, mock_md_create,
                              mock_md_free, mock_match_oom);
  g_md_null = 1;
  ASSERT_EQ(REG_ESPACE, regexec(&preg, "abc", 0, NULL, 0));

  /* match_data_create != NULL, match returns PCRE2_ERROR_NOMEMORY -> REG_ESPACE
   */
  g_md_null = 0;
  g_match_ret = -48;
  ASSERT_EQ(REG_ESPACE, regexec(&preg, "abc", 0, NULL, 0));

  posix_regex_reset_pcre2_hooks();
  regfree(&preg);
#endif

  PASS();
}

TEST test_regerror_and_regfree(void) {
  char buf[64];
  regex_t preg;

  ASSERT_EQ(0, regcomp(&preg, "abc", REG_EXTENDED));

  ASSERT(regerror(REG_NOMATCH, &preg, buf, sizeof(buf)) > 0);
  ASSERT(regerror(REG_BADPAT, &preg, buf, sizeof(buf)) > 0);
#ifdef REG_INVARG
  ASSERT(regerror(REG_INVARG, &preg, buf, sizeof(buf)) > 0);
#endif
  ASSERT(regerror(REG_ESPACE, &preg, buf, sizeof(buf)) > 0);
  ASSERT(regerror(REG_NOMATCH, &preg, NULL, 0) > 0);
  ASSERT(regerror(REG_NOMATCH, &preg, buf, 0) > 0);

  regfree(&preg);

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
  memset(&preg, 0, sizeof(preg));
  ASSERT(regerror(9999, NULL, buf, sizeof(buf)) > 0);
  ASSERT(regerror(REG_BADPAT, NULL, buf, sizeof(buf)) > 0);
  ASSERT(regerror(REG_NOMATCH, NULL, NULL, 10) > 0);
  regfree(NULL);
  regfree(&preg);
#endif

  PASS();
}

SUITE(suite_posix_regex_core) {
  RUN_TEST(test_posix_regex_init);
  RUN_TEST(test_regcomp_exec);
  RUN_TEST(test_regexec_startend_and_errors);
  RUN_TEST(test_regerror_and_regfree);
}
