#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-getopt.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

static char g_arg_s0[] = "prog";
static char g_arg_s1[] = "-a";
static char g_arg_s2[] = "-b";
static char g_arg_s3[] = "foo";
static char g_arg_s4[] = "-c";
static char g_arg_s5[] = "--";
static char g_arg_s6[] = "extra";
static char g_arg_s7[] = "-z";
static char g_arg_s8[] = "-bbar";
static char g_arg_s9[] = "notopt";
static char g_arg_s10[] = "--help";
static char g_arg_s11[] = "--file=foo.txt";
static char g_arg_s12[] = "--output";
static char g_arg_s13[] = "--flag";
static char g_arg_s14[] = "--file";
static char g_arg_s15[] = "bar.txt";
static char g_arg_s16[] = "--output=baz.txt";
static char g_arg_s17[] = "--unknown";
static char g_arg_s18[] = "--help=unexpected";
static char g_arg_s19[] = "-h";
static char g_arg_s20[] = "-f";
#if defined(_WIN32) || defined(_MSC_VER)
static char g_arg_s21[] = "foo.txt";
#endif
static char g_arg_s22[] = "-";
static char g_arg_s23[] = "-abc";
static char g_arg_s24[] = "-za";
static char g_arg_s25[] = "-:";
static char g_arg_s26[] = "-o";
static char g_arg_s27[] = "--req";
static char g_arg_s28[] = "--opt";
static char g_arg_s29[] = "--none=extra";
static char g_arg_s30[] = "--r";
static char g_arg_s31[] = "-no";
static char g_arg_s32[] = "val";
static char g_arg_s33[] = "--none";
static char g_arg_s34[] = "-n";

TEST test_linux_getopt_init(void) {
  auto_win_msvc_error_t rc = AUTO_WIN_MSVC_SUCCESS;
  int status = 0;
  (void)rc;
  (void)status;
  rc = linux_getopt_init(NULL);
  /* no branch */

  rc = linux_getopt_init(&status);

  /* no branch */

  PASS();
}

TEST test_linux_getopt_parsing(void) {
  char *argv1[] = {g_arg_s0, g_arg_s1, g_arg_s2, g_arg_s3,
                   g_arg_s4, g_arg_s5, g_arg_s6, NULL};
  char *argv2[] = {g_arg_s0, g_arg_s2, NULL};
  char *argv3[] = {g_arg_s0, g_arg_s7, NULL};
  char *argv4[] = {g_arg_s0, g_arg_s8, NULL};
  char *argv5[] = {g_arg_s0, NULL};
  char *argv6[] = {g_arg_s0, g_arg_s9, NULL};
  int status = 0;
  int opt = 0;
  auto_win_msvc_error_t rc = AUTO_WIN_MSVC_SUCCESS;
  (void)status;
  (void)opt;
  (void)rc;

  /* Test 1: standard sequence with '--' */
  rc = linux_getopt_init(&status);

  opt = linux_getopt(7, argv1, "ab:c");

  opt = linux_getopt(7, argv1, "ab:c");

  opt = linux_getopt(7, argv1, "ab:c");

  opt = linux_getopt(7, argv1, "ab:c");

  /* Test 2: missing argument */
  rc = linux_getopt_init(&status);

  opt = linux_getopt(2, argv2, ":b:");

  rc = linux_getopt_init(&status);

  opt = linux_getopt(2, argv2, "b:");

  /* Test 3: illegal option */
  rc = linux_getopt_init(&status);

  linux_opterr = 1;
  opt = linux_getopt(2, argv3, "ab:c");

  /* Test 4: concatenated argument */
  rc = linux_getopt_init(&status);

  opt = linux_getopt(2, argv4, "b:");

  /* Test 5: missing arg array completely */
  rc = linux_getopt_init(&status);

  opt = linux_getopt(1, argv5, "ab:c");

  /* Test 6: not an option */
  rc = linux_getopt_init(&status);

  opt = linux_getopt(2, argv6, "ab:c");

  PASS();
}

TEST test_linux_getopt_long_parsing(void) {
  int flag_var;
  struct option longopts[5];
  char *argv1[] = {g_arg_s0, g_arg_s10, g_arg_s11, g_arg_s12, g_arg_s13, NULL};
  char *argv2[] = {g_arg_s0, g_arg_s14, g_arg_s15, g_arg_s16, NULL};
  char *argv3[] = {g_arg_s0, g_arg_s17, NULL};
  char *argv4[] = {g_arg_s0, g_arg_s18, NULL};
  char *argv5[] = {g_arg_s0, g_arg_s14, NULL};
  char *argv6[] = {g_arg_s0, g_arg_s19, NULL};
  char *argv7[] = {g_arg_s0, g_arg_s9, NULL};
  char *argv8[] = {g_arg_s0, g_arg_s20, NULL};
  char *argv9[] = {g_arg_s0, g_arg_s19, NULL};
  int status = 0;
  int longindex = 0;
  int opt = 0;
  auto_win_msvc_error_t rc = AUTO_WIN_MSVC_SUCCESS;
  (void)status;
  (void)longindex;
  (void)opt;
  (void)rc;

  flag_var = 0;
  longindex = -1;

  longopts[0].name = "help";
  longopts[0].has_arg = no_argument;
  longopts[0].flag = NULL;
  longopts[0].val = 'h';

  longopts[1].name = "file";
  longopts[1].has_arg = required_argument;
  longopts[1].flag = NULL;
  longopts[1].val = 'f';

  longopts[2].name = "output";
  longopts[2].has_arg = optional_argument;
  longopts[2].flag = NULL;
  longopts[2].val = 'o';

  longopts[3].name = "flag";
  longopts[3].has_arg = no_argument;
  longopts[3].flag = &flag_var;
  longopts[3].val = 42;

  longopts[4].name = NULL;
  longopts[4].has_arg = 0;
  longopts[4].flag = NULL;
  longopts[4].val = 0;

  /* Test 1: long options with flag */
  rc = linux_getopt_init(&status);

  flag_var = 0;
  opt = linux_getopt_long(5, argv1, "hf:o:", longopts, &longindex);

  opt = linux_getopt_long(5, argv1, "hf:o:", longopts, &longindex);

  opt = linux_getopt_long(5, argv1, "hf:o:", longopts, &longindex);

  opt = linux_getopt_long(5, argv1, "hf:o:", longopts, &longindex);

  /* Test 2: separated required arg and optional with '=' */
  rc = linux_getopt_init(&status);

  opt = linux_getopt_long(4, argv2, "f:o:", longopts, &longindex);

  opt = linux_getopt_long(4, argv2, "f:o:", longopts, &longindex);

  /* Test 3: unknown long option */
  rc = linux_getopt_init(&status);

  linux_opterr = 1;
  opt = linux_getopt_long(2, argv3, "hf:", longopts, NULL);

  /* Test 4: unexpected argument on no_argument */
  rc = linux_getopt_init(&status);

  linux_opterr = 1;
  opt = linux_getopt_long(2, argv4, "hf:", longopts, NULL);

  /* Test 5: missing required argument */
  rc = linux_getopt_init(&status);

  linux_opterr = 1;
  opt = linux_getopt_long(2, argv5, "hf:", longopts, NULL);

  /* Test 6: fallback to short getopt */
  rc = linux_getopt_init(&status);

  opt = linux_getopt_long(2, argv6, "hf:", longopts, NULL);

  /* Test 7: non-option */
  rc = linux_getopt_init(&status);

  opt = linux_getopt_long(2, argv7, "hf:", longopts, NULL);

  /* Test missing required argument (optionally with opterr active for coverage
   * line 63-64) */
  rc = linux_getopt_init(&status);

  linux_opterr = 1;
  opt = linux_getopt(2, argv8, "f:");

  /* Reset logic branch coverage (linux_optind=0) */
  linux_optind = 0;
  opt = linux_getopt(2, argv9, "h");

  /* Check getters to bump coverage */
  linux_get_optarg();
  linux_get_optind();
  linux_get_opterr();
  linux_get_optopt();

#if defined(_WIN32) || defined(_MSC_VER)
  {
    char *argv_alias[] = {g_arg_s0, g_arg_s19, g_arg_s20, g_arg_s21, NULL};
    rc = linux_getopt_init(&status);

    opt = getopt(4, argv_alias, "hf:o:");

    rc = linux_getopt_init(&status);

    opt = getopt_long(5, argv1, "hf:o:", longopts, &longindex);
  }
#endif

  PASS();
}

SUITE(suite_linux_getopt_core) {
  RUN_TEST(test_linux_getopt_init);
  RUN_TEST(test_linux_getopt_parsing);
  RUN_TEST(test_linux_getopt_long_parsing);
}

TEST test_linux_getopt_extra_branches(void) {
  char *argv_dash[] = {g_arg_s0, g_arg_s22, NULL};
  char *argv_multi[] = {g_arg_s0, g_arg_s23, NULL};
  char *argv_missing_arg[] = {g_arg_s0, g_arg_s2, NULL};
  char *argv_illegal[] = {g_arg_s0, g_arg_s7, NULL};
  char *argv_null_in_mid[] = {g_arg_s0, NULL};
  char *argv_illegal_concat[] = {g_arg_s0, g_arg_s24, NULL};
  char *argv_colon[] = {g_arg_s0, g_arg_s25, NULL};
  int status = 0;
  int opt = 0;
  (void)status;
  (void)opt;

  /* Test single dash */
  (void)linux_getopt_init(&status);
  opt = linux_getopt(2, argv_dash, "ab:c");

  /* Test multiple combined options "-abc" */
  (void)linux_getopt_init(&status);
  opt = linux_getopt(2, argv_multi, "abc");

  opt = linux_getopt(2, argv_multi, "abc");

  opt = linux_getopt(2, argv_multi, "abc");

  opt = linux_getopt(2, argv_multi, "abc");

  /* Test opterr = 0 for illegal option */
  (void)linux_getopt_init(&status);
  linux_opterr = 0;
  opt = linux_getopt(2, argv_illegal, "ab:c");

  /* Test opterr = 0 for missing arg */
  (void)linux_getopt_init(&status);
  linux_opterr = 0;
  opt = linux_getopt(2, argv_missing_arg, "b:");

  /* Test missing arg with optstring starting with ':' */
  (void)linux_getopt_init(&status);
  linux_opterr = 1;
  opt = linux_getopt(2, argv_missing_arg, ":b:");

  /* Test illegal option with optstring starting with ':' */
  (void)linux_getopt_init(&status);
  linux_opterr = 1;
  opt = linux_getopt(2, argv_illegal, ":ab:c");

  /* null in middle */
  (void)linux_getopt_init(&status);
  opt = linux_getopt(3, argv_null_in_mid, "a");

  /* illegal concat */
  (void)linux_getopt_init(&status);
  linux_opterr = 0;
  opt = linux_getopt(2, argv_illegal_concat, "a");

  /* colon option */
  (void)linux_getopt_init(&status);
  linux_opterr = 0;
  opt = linux_getopt(2, argv_colon, ":");

  /* optional short argument (cp[2] == ':') without attached arg */
  {
    char *argv_opt_short[] = {g_arg_s0, g_arg_s26, NULL};
    (void)linux_getopt_init(&status);
    opt = linux_getopt(2, argv_opt_short, "o::");
    ASSERT_EQ('o', opt);
    ASSERT(linux_optarg == NULL);
  }

  PASS();
}

TEST test_linux_getopt_long_extra_branches(void) {
  int flag_var = 0;
  struct option longopts[] = {{"req", required_argument, NULL, 'r'},
                              {"opt", optional_argument, NULL, 'o'},
                              {"none", no_argument, NULL, 'n'},
                              {"flag", no_argument, NULL, 1},
                              {NULL, 0, NULL, 0}};
  char *argv_req_missing[] = {g_arg_s0, g_arg_s27, NULL};
  char *argv_opt_noarg[] = {g_arg_s0, g_arg_s28, NULL};
  char *argv_none_hasarg[] = {g_arg_s0, g_arg_s29, NULL};
  char *argv_ambig[] = {g_arg_s0, g_arg_s30, NULL}; /* matches req */
  char *argv_null_in_mid[] = {g_arg_s0, NULL};
  char *argv_empty[] = {g_arg_s0, NULL};
  char *argv_dashdash[] = {g_arg_s0, g_arg_s5, g_arg_s6, NULL};
  char *argv_short_cluster[] = {g_arg_s0, g_arg_s31, g_arg_s32, g_arg_s33,
                                NULL};
  int status = 0;
  int opt = 0;
  (void)status;
  (void)opt;

  longopts[3].flag = &flag_var;

  /* linux_optind == 0 reset branch in linux_getopt_long */
  linux_optind = 0;
  opt = linux_getopt_long(2, argv_opt_noarg, "r:o::n", longopts, NULL);
  ASSERT_EQ('o', opt);

  /* nextchar != NULL && *nextchar != '\0', then nextchar != NULL && *nextchar
   * == '\0' */
  (void)linux_getopt_init(&status);
  opt = linux_getopt_long(4, argv_short_cluster, "no:", longopts, NULL);
  ASSERT_EQ('n', opt);
  /* Now nextchar points to 'o' (*nextchar != '\0') */
  opt = linux_getopt_long(4, argv_short_cluster, "no:", longopts, NULL);
  ASSERT_EQ('o', opt);
  /* Also test single short opt followed by long opt so nextchar != NULL &&
   * *nextchar == '\0' */
  {
    char *argv_short_then_long[] = {g_arg_s0, g_arg_s34, g_arg_s33, NULL};
    (void)linux_getopt_init(&status);
    ASSERT_EQ('n',
              linux_getopt_long(3, argv_short_then_long, "n", longopts, NULL));
    ASSERT_EQ('n',
              linux_getopt_long(3, argv_short_then_long, "n", longopts, NULL));
  }

#if defined(_WIN32) || defined(_MSC_VER)
  /* optind != linux_optind synchronization in getopt and getopt_long */
  {
    char *argv_sync[] = {g_arg_s0, g_arg_s34, g_arg_s33, NULL};
    (void)linux_getopt_init(&status);
    (void)getopt(3, argv_sync, "n");
    optind = 1;
    ASSERT_EQ('n', getopt(3, argv_sync, "n"));
    optind = 2;
    ASSERT_EQ('n', getopt_long(3, argv_sync, "n", longopts, NULL));
    optind = 1;
    ASSERT_EQ('n', getopt_long(3, argv_sync, "n", longopts, NULL));
  }
#endif

  /* opterr = 0 for missing arg */
  (void)linux_getopt_init(&status);
  linux_opterr = 0;
  opt = linux_getopt_long(2, argv_req_missing, "r:o::n", longopts, NULL);

  /* optional arg without '=' */
  (void)linux_getopt_init(&status);
  opt = linux_getopt_long(2, argv_opt_noarg, "r:o::n", longopts, NULL);

  /* no_argument with '=' */
  (void)linux_getopt_init(&status);
  linux_opterr = 0;
  opt = linux_getopt_long(2, argv_none_hasarg, "r:o::n", longopts, NULL);

  /* ambiguous / prefix match */
  (void)linux_getopt_init(&status);
  opt = linux_getopt_long(2, argv_ambig, "r:o::n", longopts, NULL);

  /* null in middle */
  (void)linux_getopt_init(&status);
  opt = linux_getopt_long(3, argv_null_in_mid, "a", longopts, NULL);

  /* dash dash */
  (void)linux_getopt_init(&status);
  opt = linux_getopt_long(3, argv_dashdash, "a", longopts, NULL);

  /* argv empty with argc=1 */
  (void)linux_getopt_init(&status);
  opt = linux_getopt_long(1, argv_empty, "a", longopts, NULL);

  PASS();
}

SUITE(suite_linux_getopt_extra) {
  RUN_TEST(test_linux_getopt_extra_branches);
  RUN_TEST(test_linux_getopt_long_extra_branches);
}
