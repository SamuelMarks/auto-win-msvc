#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-strings.h"
/* clang-format on */

TEST test_strings(void) {
  ASSERT_EQ(0, strcasecmp("a", "A"));
  PASS();
}

TEST test_strtok_r(void) {
  char str[32];
  char *saveptr;
  char *token;

  saveptr = NULL;
#if defined(_MSC_VER)
  strcpy_s(str, sizeof(str), "apple,banana,cherry");
#else
  strcpy(str, "apple,banana,cherry");
#endif

  token = strtok_r(str, ",", &saveptr);
  ASSERT_STR_EQ("apple", token);

  token = strtok_r(NULL, ",", &saveptr);
  ASSERT_STR_EQ("banana", token);

  token = strtok_r(NULL, ",", &saveptr);
  ASSERT_STR_EQ("cherry", token);

  token = strtok_r(NULL, ",", &saveptr);
  ASSERT_EQ(NULL, token);

  PASS();
}

TEST test_ffs(void) { SKIP(); /* Generated stub for ffs */ }

TEST test_ffsl(void) { SKIP(); /* Generated stub for ffsl */ }

TEST test_ffsll(void) { SKIP(); /* Generated stub for ffsll */ }

SUITE(suite_posix_strings_core) {
  RUN_TEST(test_strings);
  RUN_TEST(test_strtok_r);
  RUN_TEST(test_ffs);
  RUN_TEST(test_ffsl);
  RUN_TEST(test_ffsll);
}
