#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "linux-getopt.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

TEST test_linux_getopt_init(void) {
  enum linux_getopt_error_code rc;
  int status;

  status = 0;
  rc = linux_getopt_init(NULL);
  if (rc != LINUX_GETOPT_ERROR_NULL_POINTER) {
    printf("Expected LINUX_GETOPT_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = linux_getopt_init(&status);
  if (rc != LINUX_GETOPT_SUCCESS) {
    printf("linux_getopt_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_linux_getopt_parsing(void) {
  char *argv1[] = {"prog", "-a", "-b", "foo", "-c", "--", "extra", NULL};
  char *argv2[] = {"prog", "-b", NULL};
  char *argv3[] = {"prog", "-z", NULL};
  char *argv4[] = {"prog", "-bbar", NULL};
  int status;
  int opt;

  /* Test 1: standard sequence with '--' */
  linux_getopt_init(&status);
  opt = linux_getopt(7, argv1, "ab:c");
  ASSERT_EQ('a', opt);
  opt = linux_getopt(7, argv1, "ab:c");
  ASSERT_EQ('b', opt);
  ASSERT_STR_EQ("foo", linux_get_optarg());
  opt = linux_getopt(7, argv1, "ab:c");
  ASSERT_EQ('c', opt);
  opt = linux_getopt(7, argv1, "ab:c");
  ASSERT_EQ(-1, opt);

  /* Test 2: missing argument */
  linux_getopt_init(&status);
  opt = linux_getopt(2, argv2, ":b:");
  ASSERT_EQ(':', opt);

  linux_getopt_init(&status);
  opt = linux_getopt(2, argv2, "b:");
  ASSERT_EQ('?', opt);

  /* Test 3: illegal option */
  linux_getopt_init(&status);
  opt = linux_getopt(2, argv3, "ab:c");
  ASSERT_EQ('?', opt);

  /* Test 4: concatenated argument */
  linux_getopt_init(&status);
  opt = linux_getopt(2, argv4, "b:");
  ASSERT_EQ('b', opt);
  ASSERT_STR_EQ("bar", linux_get_optarg());

  PASS();
}

TEST test_linux_getopt_long_parsing(void) {
  int flag_var;
  struct option longopts[5];
  char *argv1[] = {"prog",     "--help", "--file=foo.txt",
                   "--output", "--flag", NULL};
  char *argv2[] = {"prog", "--file", "bar.txt", "--output=baz.txt", NULL};
  char *argv3[] = {"prog", "--unknown", NULL};
  char *argv4[] = {"prog", "--help=unexpected", NULL};
  char *argv5[] = {"prog", "--file", NULL};
  int status;
  int longindex;
  int opt;

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
  linux_getopt_init(&status);
  flag_var = 0;
  opt = linux_getopt_long(5, argv1, "hf:o:", longopts, &longindex);
  ASSERT_EQ('h', opt);
  ASSERT_EQ(0, longindex);

  opt = linux_getopt_long(5, argv1, "hf:o:", longopts, &longindex);
  ASSERT_EQ('f', opt);
  ASSERT_EQ(1, longindex);
  ASSERT_STR_EQ("foo.txt", linux_get_optarg());

  opt = linux_getopt_long(5, argv1, "hf:o:", longopts, &longindex);
  ASSERT_EQ('o', opt);
  ASSERT_EQ(2, longindex);
  ASSERT_EQ(NULL, linux_get_optarg());

  opt = linux_getopt_long(5, argv1, "hf:o:", longopts, &longindex);
  ASSERT_EQ(0, opt);
  ASSERT_EQ(3, longindex);
  ASSERT_EQ(42, flag_var);

  /* Test 2: separated required arg and optional with '=' */
  linux_getopt_init(&status);
  opt = linux_getopt_long(4, argv2, "f:o:", longopts, &longindex);
  ASSERT_EQ('f', opt);
  ASSERT_STR_EQ("bar.txt", linux_get_optarg());

  opt = linux_getopt_long(4, argv2, "f:o:", longopts, &longindex);
  ASSERT_EQ('o', opt);
  ASSERT_STR_EQ("baz.txt", linux_get_optarg());

  /* Test 3: unknown long option */
  linux_getopt_init(&status);
  opt = linux_getopt_long(2, argv3, "hf:", longopts, NULL);
  ASSERT_EQ('?', opt);

  /* Test 4: unexpected argument on no_argument */
  linux_getopt_init(&status);
  opt = linux_getopt_long(2, argv4, "hf:", longopts, NULL);
  ASSERT_EQ('?', opt);

  /* Test 5: missing required argument */
  linux_getopt_init(&status);
  opt = linux_getopt_long(2, argv5, "hf:", longopts, NULL);
  ASSERT_EQ('?', opt);

#if defined(_WIN32) || defined(_MSC_VER)
  {
    char *argv_alias[] = {"prog", "-h", "-f", "foo.txt", NULL};
    linux_getopt_init(&status);
    opt = getopt(4, argv_alias, "hf:o:");
    ASSERT_EQ('h', opt);

    linux_getopt_init(&status);
    opt = getopt_long(5, argv1, "hf:o:", longopts, &longindex);
    ASSERT_EQ('h', opt);
  }
#endif

  PASS();
}

SUITE(suite_linux_getopt_core) {
  RUN_TEST(test_linux_getopt_init);
  RUN_TEST(test_linux_getopt_parsing);
  RUN_TEST(test_linux_getopt_long_parsing);
}
